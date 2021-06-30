/*
 * This is a simple concurrent thead safe queue implementation, it does not intend
 * to be exhaustive but it's nice :)
 */

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "gnl_queue_t.c"
#include "../include/gnl_ts_queue_t.h"
#include <gnl_macro_beg.h>

struct gnl_ts_queue_t {
    pthread_mutex_t mtx;
    gnl_queue_t *q;
};

gnl_ts_queue_t *gnl_ts_queue_init() {
    gnl_ts_queue_t *queue = (gnl_ts_queue_t*)malloc(sizeof(gnl_ts_queue_t));

    GNL_NULL_CHECK(queue, ENOMEM, NULL)

    int pthread_res = pthread_mutex_init(&(queue->mtx), NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    queue->q = gnl_queue_init();
    GNL_NULL_CHECK(queue->q, ENOMEM, NULL)

    return queue;
}

int gnl_ts_queue_destroy(gnl_ts_queue_t *q, void (*destroy)(void *data)) {
    int pthread_res;
    pthread_mutex_t mtx;

    pthread_res = pthread_mutex_init(&mtx, NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    pthread_res = pthread_mutex_lock(&mtx);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    if (q == NULL) {
        return 0;
    }

    gnl_queue_destroy((q->q), destroy);

    pthread_res = pthread_mutex_destroy(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    free(q);

    pthread_res = pthread_mutex_unlock(&mtx);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    pthread_res = pthread_mutex_destroy(&mtx);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    return 0;
}

int gnl_ts_queue_enqueue(gnl_ts_queue_t *q, void *el) {
    GNL_NULL_CHECK(q, EINVAL, -1)

    int pthread_res;

    pthread_res = pthread_mutex_lock(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    int res = gnl_queue_enqueue(q->q, el);
    GNL_MINUS1_CHECK(res, errno, -1)

    pthread_res = pthread_mutex_unlock(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    return 0;
}

void *gnl_ts_queue_dequeue(gnl_ts_queue_t *q) {
    GNL_NULL_CHECK(q, EINVAL, NULL)

    int pthread_res;

    if (gnl_ts_queue_size(q) == 0) {
        return NULL;
    }

    pthread_res = pthread_mutex_lock(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    void *temp = gnl_queue_dequeue(q->q);

    pthread_res = pthread_mutex_unlock(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    return temp;
}

unsigned long gnl_ts_queue_size(gnl_ts_queue_t *q) {
    GNL_NULL_CHECK(q, EINVAL, -1)

    int pthread_res;

    pthread_res = pthread_mutex_lock(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    unsigned long temp = q->q->size;

    pthread_res = pthread_mutex_unlock(&(q->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    return temp;
}

#include <gnl_macro_end.h>