/*
 * This is a simple concurrent thead safe queue implementation, it does not intend
 * to be exhaustive but it's nice :)
 */

#include <stdlib.h>
#include <pthread.h>
#include "gnl_queue_t.c"
#include "../includes/gnl_ts_queue_t.h"

const int GNL_TS_QUEUE_FIFO = GNL_QUEUE_FIFO;
const int GNL_TS_QUEUE_LIFO = GNL_QUEUE_LIFO;

struct gnl_ts_queue_t {
    pthread_mutex_t mtx;
    gnl_queue_t *q;
};

gnl_ts_queue_t *gnl_ts_queue_init(int type) {
    if (type != GNL_TS_QUEUE_FIFO && type != GNL_TS_QUEUE_LIFO) {
        errno = EINVAL;
        return NULL;
    }

    gnl_ts_queue_t *queue = (gnl_ts_queue_t*)malloc(sizeof(gnl_ts_queue_t));
    if (queue == NULL) {
        perror("malloc");

        return NULL;
    }

    int pthread_res = pthread_mutex_init(&(queue->mtx), NULL);
    if (pthread_res == -1) {
        perror("pthread_mutex_init");

        return NULL;
    }

    queue->q = gnl_queue_init(type);
    if (queue->q == NULL) {
        perror("gnl_queue_init");

        return NULL;
    }

    return queue;
}

int gnl_ts_queue_destroy(gnl_ts_queue_t **q) {
    int pthread_res;
    pthread_mutex_t mtx;

    pthread_res = pthread_mutex_init(&mtx, NULL);
    if (pthread_res == -1) {
        perror("pthread_mutex_init");

        return -1;
    }

    pthread_res = pthread_mutex_lock(&mtx);
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return -1;
    }

    if (q == NULL) {
        return 0;
    }

    gnl_queue_destroy(&((*q)->q));
    free(*q);

    pthread_res = pthread_mutex_unlock(&mtx);
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return 0;
}

int gnl_ts_queue_push(gnl_ts_queue_t **q, void *el) {
    int pthread_res;

    pthread_res = pthread_mutex_lock(&((*q)->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return -1;
    }

    int res = gnl_queue_push(&((*q)->q), el);
    if (res == -1) {
        perror("gnl_queue_push");

        return -1;
    }

    pthread_res = pthread_mutex_unlock(&((*q)->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return 0;
}

void *gnl_ts_queue_pop(gnl_ts_queue_t **q) {
    int pthread_res;

    if (gnl_ts_queue_size(*q) == 0) {
        return NULL;
    }

    pthread_res = pthread_mutex_lock(&((*q)->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return NULL;
    }

    void *temp = gnl_queue_pop(&((*q)->q));

    pthread_res = pthread_mutex_unlock(&((*q)->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return NULL;
    }

    return temp;
}

int gnl_ts_queue_size(const gnl_ts_queue_t *q) {
    return q->q->size;
}