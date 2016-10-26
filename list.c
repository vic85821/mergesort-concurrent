#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

llist_t *list_new(int size)
{
    /* allocate list */
    llist_t *list = malloc(sizeof(llist_t));
    list->head = malloc(sizeof(node_t) * size);
    list->size = 0;
    return list;
}

/*
 * list_add inserts a new node with the given value val in the list
 * (if the value was absent) or does nothing (if the value is already present).
 */
int list_add(llist_t *list, val_t val)
{
    node_t e;
    strcpy(e.data,val);
    e.index = list->size;
    list->head[list->size] = e;
    list->size++;

    return 0;
}

void list_print(llist_t *list)
{
    int pos = 0;
    /* FIXME: we have to validate the sorted results in advance. */
#ifndef TEST
    printf("\nsorted results:\n");
    while (pos < list->size) {
        printf("%s\n", list->head[pos++].data);
    }
    printf("\n");
#endif
}
