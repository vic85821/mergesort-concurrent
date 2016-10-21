#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "threadpool.h"
#include "list.h"

#define USAGE "usage: ./sort [thread_count] [input_count]\n"

struct {
    pthread_mutex_t mutex;
    int cut_thread_count;
} data_context;

static llist_t *tmp_list;
static llist_t *the_list = NULL;

static int thread_count = 0, data_count = 0;
static tpool_t *pool = NULL;

void cut_func(void *data);

llist_t *merge_list(llist_t *a, llist_t *b)
{
    llist_t *_list = malloc(sizeof(llist_t));
    _list->size = 0;

    node_t *tmp_ptr = (node_t *)
                      ((intptr_t) a->head * (a->head[0].index < b->head[0].index) +
                       (intptr_t) b->head * (b->head[0].index < a->head[0].index));

    val_t *tmp_list = malloc(sizeof(val_t) * (a->size + b->size));

    int tmp = 0;
    while (a->size && b->size) {
        llist_t *small = (llist_t *)
                         ((intptr_t) a * (strcmp(a->head[0].data,b->head[0].data) < 0 ? 1:0)+
                          (intptr_t) b * (strcmp(a->head[0].data,b->head[0].data) < 0 ? 0:1));
        strcpy(tmp_list[tmp++],small->head[0].data);
        --small->size;
        small->head = &small->head[1];
    }

    llist_t *remaining = (llist_t *) ((intptr_t) a * (a->size > 0) +
                                      (intptr_t) b * (b->size > 0));

    while(remaining->size) {
        strcpy(tmp_list[tmp++],remaining->head[0].data);
        remaining->size--;
        remaining->head = (&remaining->head[1]);
    }
    _list->head = tmp_ptr;
    for(int i = 0; i < tmp; ++i) {
        _list->size++;
        strcpy(_list->head[i].data,tmp_list[i]);
    }

    free(tmp_list);
    return _list;
}

void merge(void *data)
{
    llist_t *_list = (llist_t *) data;
    if (_list->size < (uint32_t) data_count) {
        pthread_mutex_lock(&(data_context.mutex));
        llist_t *_t = tmp_list;
        if (!_t) {
            tmp_list = _list;
            pthread_mutex_unlock(&(data_context.mutex));
        } else {
            if( !(_list->head[0].index + _list->size == tmp_list->head[0].index ||
                    _list->head[0].index == tmp_list->head[0].index + tmp_list->size)) {
                task_t *_task = (task_t *) malloc(sizeof(task_t));
                _task->func = merge;
                _task->arg = _list;
                tqueue_push(pool->queue, _task);
                pthread_mutex_unlock(&(data_context.mutex));
            } else {
                tmp_list = NULL;
                pthread_mutex_unlock(&(data_context.mutex));
                task_t *_task = (task_t *) malloc(sizeof(task_t));
                _task->func = merge;
                _task->arg = merge_list(_list, _t);
                tqueue_push(pool->queue, _task);
            }
        }
    } else {
        pthread_mutex_lock(&(data_context.mutex));
        the_list = _list;
        task_t *_task = (task_t *) malloc(sizeof(task_t));
        _task->func = NULL;
        tqueue_push(pool->queue, _task);
        list_print(_list);
        pthread_mutex_unlock(&(data_context.mutex));
    }
}

void cut_func(void *data)
{
    llist_t *list = (llist_t *) data;
    pthread_mutex_lock(&(data_context.mutex));
    if (list->size > 1 /*&& cut_count < max_cut*/) {
        ++data_context.cut_thread_count;
        pthread_mutex_unlock(&(data_context.mutex));

        /* cut list */
        int mid = list->size / 2;
        llist_t *_list = malloc(sizeof(llist_t));
        _list->head = &(list->head[mid]);
        _list->size = list->size - mid;
        list->size = mid;

        /* create new task: left */
        task_t *_task = (task_t *) malloc(sizeof(task_t));
        _task->func = cut_func;
        _task->arg = _list;
        tqueue_push(pool->queue, _task);

        /* create new task: right */
        _task = (task_t *) malloc(sizeof(task_t));
        _task->func = cut_func;
        _task->arg = list;
        tqueue_push(pool->queue, _task);
    } else {
        pthread_mutex_unlock(&(data_context.mutex));
        merge(list);
    }
}

static void *task_run(void *data)
{
    (void) data;
    while (1) {
        task_t *_task = tqueue_pop(pool->queue);
        if (_task) {
            if (!_task->func) {
                tqueue_push(pool->queue, _task);
                break;
            } else {
                _task->func(_task->arg);
                free(_task);
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        printf(USAGE);
        return -1;
    }
    thread_count = atoi(argv[1]);
    data_count = atoi(argv[2]);
    //max_cut = thread_count * (thread_count <= data_count) +
    //          data_count * (thread_count > data_count) - 1;

    if(data_count > ARRAY_MAX_SIZE) {
        printf("data size is larger than limit\n");
        return -1;
    }
    /* Read data */
    the_list = list_new(ARRAY_MAX_SIZE);

    /* FIXME: remove all all occurrences of printf and scanf
     * in favor of automated test flow.
     */
#ifndef TEST
    printf("input unsorted data line-by-line\n");
#endif
    for (int i = 0; i < data_count; ++i) {
        char *data = malloc(sizeof(char) * MAX_LAST_NAME_LEN);
        scanf("%s", data);
        list_add(the_list, data);
    }

    /* initialize tasks inside thread pool */
    pthread_mutex_init(&(data_context.mutex), NULL);
    data_context.cut_thread_count = 0;
    tmp_list = NULL;
    pool = (tpool_t *) malloc(sizeof(tpool_t));
    tpool_init(pool, thread_count, task_run);

    /* launch the first task */
    task_t *_task = (task_t *) malloc(sizeof(task_t));
    _task->func = cut_func;
    _task->arg = the_list;
    tqueue_push(pool->queue, _task);

    /* release thread pool */
    tpool_free(pool);
    return 0;
}
