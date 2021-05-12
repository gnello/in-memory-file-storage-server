/*
 * This is a simple concurrent thead safe queue implementation, it does not intend
 * to be exhaustive but it's nice :)
 */

#include <stdlib.h>
#include <pthread.h>
#include "gnl_queue_t.c"
#include "../include/gnl_ts_queue_t.h"

struct gnl_ts_queue_t {
    pthread_mutex_t mtx;
    gnl_queue_t *q;
};

gnl_ts_queue_t *gnl_ts_queue_init() {
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

    queue->q = gnl_queue_init();
    if (queue->q == NULL) {
        perror("gnl_queue_init");

        return NULL;
    }

    return queue;
}

int gnl_ts_queue_destroy(gnl_ts_queue_t *q, void (*destroy)(void *data)) {
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

    gnl_queue_destroy((q->q), destroy);

    pthread_res = pthread_mutex_destroy(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_destroy");

        return -1;
    }

    free(q);

    pthread_res = pthread_mutex_unlock(&mtx);
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    pthread_res = pthread_mutex_destroy(&mtx);
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return 0;
}

int gnl_ts_queue_enqueue(gnl_ts_queue_t *q, void *el) {
    int pthread_res;

    pthread_res = pthread_mutex_lock(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return -1;
    }

    int res = gnl_queue_enqueue(q->q, el);
    if (res == -1) {
        perror("gnl_queue_enqueue");

        return -1;
    }

    pthread_res = pthread_mutex_unlock(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return 0;
}

void *gnl_ts_queue_dequeue(gnl_ts_queue_t *q) {
    int pthread_res;

    if (gnl_ts_queue_size(q) == 0) {
        return NULL;
    }

    pthread_res = pthread_mutex_lock(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return NULL;
    }

    void *temp = gnl_queue_dequeue(q->q);

    pthread_res = pthread_mutex_unlock(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return NULL;
    }

    return temp;
}

unsigned long gnl_ts_queue_size(gnl_ts_queue_t *q) {
    int pthread_res;

    pthread_res = pthread_mutex_lock(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return -1;
    }

    unsigned long temp = q->q->size;

    pthread_res = pthread_mutex_unlock(&(q->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return temp;
}