/*
 * This is a simple queue implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdlib.h>
#include <errno.h>
#include "../includes/gnl_queue_t.h"

// the node of the queue
struct gnl_queue_node {
    void *data;
    struct gnl_queue_node *next;
};

// the queue
struct gnl_queue_t {
    struct gnl_queue_node *front;
    struct gnl_queue_node *rear;
    unsigned long size;
};

gnl_queue_t *gnl_queue_init() {
    gnl_queue_t *queue = (gnl_queue_t *)malloc(sizeof(gnl_queue_t));

    if (queue == NULL) {
        perror("malloc");

        return NULL;
    }

    // init the queue implementation data
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;

    return queue;
}

void gnl_queue_destroy(gnl_queue_t *q) {
    if (q != NULL) {
        // destroy every node
        while(q->front != NULL) {
            struct gnl_queue_node *temp = q->front;
            q->front = q->front->next;

            free(temp);
        }

        free(q);
    }
}

int gnl_queue_enqueue(gnl_queue_t *q, void *el) {
    // create the new queue node
    struct gnl_queue_node *temp = (struct gnl_queue_node *)malloc(sizeof(struct gnl_queue_node));

    if (temp == NULL) {
        perror("malloc");

        return -1;
    }

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

void *gnl_queue_dequeue(gnl_queue_t *q) {
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

unsigned long gnl_queue_size(const gnl_queue_t *q) {
    return q->size;
}