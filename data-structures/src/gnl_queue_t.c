/*
 * This is a simple queue implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdlib.h>
#include <errno.h>
#include "gnl_list_t.c" //TODO: include .h (già compilato?)
#include "../includes/gnl_queue_t.h"

const int GNL_FIFO_QUEUE = 0;
const int GNL_LIFO_QUEUE = 1;

struct gnl_queue_t {
    gnl_list_t *list;
    size_t size;
    int type;
};

gnl_queue_t *gnl_queue_init(int type) {
    if (type != GNL_FIFO_QUEUE && type != GNL_LIFO_QUEUE) {
        errno = EINVAL;
        return NULL;
    }

    gnl_queue_t *queue = (gnl_queue_t *)malloc(sizeof(gnl_queue_t));

    if (queue == NULL) {
        perror("malloc");

        return NULL;
    }

    gnl_list_t *list = NULL;
    queue->list = list;
    queue->size = 0;
    queue->type = type;

    return queue;
}

void gnl_queue_destroy(gnl_queue_t **q) {
    if (*q != NULL) {
        gnl_list_destroy(&((*q)->list));

        free(*q);
    }
}

int gnl_queue_push(gnl_queue_t **q, void *el) {
    int res = -1;

    if ((*q)->type == GNL_FIFO_QUEUE) {
        res = gnl_list_append(&((*q)->list), el);
    } else if ((*q)->type == GNL_LIFO_QUEUE) {
        res = gnl_list_insert(&((*q)->list), el);
    }

    if (res < 0) {
        perror((*q)->type == GNL_FIFO_QUEUE ? "gnl_list_append" : "gnl_list_insert");

        return -1;
    }

    (*q)->size++;

    return 0;
}

void *gnl_queue_pop(gnl_queue_t **q) {
    if ((*q)->size == 0) {
        return NULL;
    }

    void *temp = (*q)->list->el;

    gnl_list_delete(&((*q)->list), (*q)->list->el);
    (*q)->size--;

    return temp;
}

int gnl_queue_size(const gnl_queue_t *q) {
    return q->size;
}