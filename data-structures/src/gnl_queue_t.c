/*
 * This is a simple queue implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/gnl_queue_t.h"
#include <gnl_macro_beg.h>

// the node of the queue
struct gnl_queue_node {
    void *data;
    struct gnl_queue_node *next;
};

/**
 * front    The head pointer of the queue.
 * rear     The tail pointer of the queue.
 * size     The size of the queue.
 */
struct gnl_queue_t {
    struct gnl_queue_node *front;
    struct gnl_queue_node *rear;
    unsigned long size;
};

/**
 * {@inheritDoc}
 */
struct gnl_queue_t *gnl_queue_init() {
    struct gnl_queue_t *queue = (struct gnl_queue_t *)malloc(sizeof(struct gnl_queue_t));

    GNL_NULL_CHECK(queue, ENOMEM, NULL)

    // init the queue implementation data
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;

    return queue;
}

/**
 * {@inheritDoc}
 */
void gnl_queue_destroy(struct gnl_queue_t *q, void (*destroy)(void *data)) {
    if (q != NULL) {
        // destroy every node
        while(q->front != NULL) {
            struct gnl_queue_node *temp = q->front;
            q->front = q->front->next;

            if (destroy != NULL) {
                destroy(temp->data);
            }

            free(temp);
        }

        free(q);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_queue_enqueue(struct gnl_queue_t *q, void *el) {
    GNL_NULL_CHECK(q, EINVAL, -1)

    // create the new queue node
    struct gnl_queue_node *temp = (struct gnl_queue_node *)malloc(sizeof(struct gnl_queue_node));

    GNL_NULL_CHECK(temp, ENOMEM, -1)

    temp->data = el;
    temp->next = NULL;

    // if queue is empty put the node into front and into rear
    if (q->rear == NULL) {
        q->front = temp;
        q->rear = temp;
    } else {
        // add the new queue node at the end of queue and change the rear
        q->rear->next = temp;
        q->rear = temp;
    }

    q->size++;

    return 0;
}

/**
 * {@inheritDoc}
 */
void *gnl_queue_dequeue(struct gnl_queue_t *q) {
    GNL_NULL_CHECK(q, EINVAL, NULL)

    // if queue is empty return NULL.
    if (q->front == NULL) {
        return NULL;
    }

    // move front to the next node
    struct gnl_queue_node *temp = q->front;

    q->front = q->front->next;

    // if front becomes NULL then change rear also as NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    q->size--;

    void *data = temp->data;

    free(temp);

    return data;
}

/**
 * {@inheritDoc}
 */
unsigned long gnl_queue_size(const struct gnl_queue_t *q) {
    GNL_NULL_CHECK(q, EINVAL, -1)

    return q->size;
}

#include <gnl_macro_end.h>