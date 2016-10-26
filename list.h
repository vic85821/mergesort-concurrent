#ifndef LLIST_H_
#define LLIST_H_

#include <stdint.h>

#define MAX_LAST_NAME_LEN 20
<<<<<<< HEAD
#define ARRAY_MAX_SIZE 2000
=======
#define ARRAY_MAX_SIZE 349900
    >>>>>>> 2151827ac6d7e41e843d5f98f9e799ef3e09e22f

    typedef char val_t[MAX_LAST_NAME_LEN];

typedef struct node {
    val_t data;
    int index;
} node_t;

typedef struct llist {
    node_t *head;
    uint32_t size;
} llist_t;

llist_t *list_new(int size);
int list_add(llist_t *the_list, val_t val);
void list_print(llist_t *the_list);

#endif
