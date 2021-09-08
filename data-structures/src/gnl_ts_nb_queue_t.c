/*
 * This is a simple concurrent thead-safe non-blocking queue implementation, it does not intend
 * to be exhaustive.
 */

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "gnl_queue_t.c"
#include "../include/gnl_ts_nb_queue_t.h"
#include <gnl_macro_beg.h>

#define GNL_LOCK_ACQUIRE(lock, return_value) { \
    int lock_acquire_res = pthread_mutex_lock(lock); \
    GNL_MINUS1_CHECK(lock_acquire_res, errno, return_value) \
}

#define GNL_LOCK_RELEASE(lock, return_value) { \
    int lock_release_res = pthread_mutex_unlock(lock); \
    GNL_MINUS1_CHECK(lock_release_res, errno, return_value) \
}

#define GNL_RELEASE_AND_RETURN_ON_ERROR(actual, expected_error, lock, errno_code, return_value) { \
    if (actual == expected_error) {                                               \
        errno = errno_code;                                                \
        GNL_LOCK_RELEASE(lock, return_value) \
\
        return return_value; \
    } \
}

/**
 * mtx  The mutex var of the thread-safe blocking bounded queue.
 * q    The queue data-structure.
 */
struct gnl_ts_nb_queue_t {
    pthread_mutex_t mtx;
    struct gnl_queue_t *q;
};

/**
 * {@inheritDoc}
 */
struct gnl_ts_nb_queue_t *gnl_ts_nb_queue_init() {
    struct gnl_ts_nb_queue_t *queue = (struct gnl_ts_nb_queue_t*)malloc(sizeof(struct gnl_ts_nb_queue_t));
    GNL_NULL_CHECK(queue, ENOMEM, NULL)

    // initialize lock
    int pthread_res = pthread_mutex_init(&(queue->mtx), NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    // initialize queue
    queue->q = gnl_queue_init();
    GNL_NULL_CHECK(queue->q, ENOMEM, NULL)

    return queue;
}

/**
 * {@inheritDoc}
 */
int gnl_ts_nb_queue_destroy(struct gnl_ts_nb_queue_t *q, void (*destroy)(void *data)) {
    int res;

    if (q == NULL) {
        return 0;
    }

    gnl_queue_destroy((q->q), destroy);

    res = pthread_mutex_destroy(&(q->mtx));
    GNL_MINUS1_CHECK(res, errno, -1)

    free(q);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_ts_nb_queue_enqueue(struct gnl_ts_nb_queue_t *q, void *el) {
    int res;

    GNL_LOCK_ACQUIRE(&(q->mtx), -1)
    GNL_RELEASE_AND_RETURN_ON_ERROR(q, NULL, &(q->mtx), EINVAL, -1)

    // insert element
    res = gnl_queue_enqueue(q->q, el);
    GNL_RELEASE_AND_RETURN_ON_ERROR(res, -1, &(q->mtx), errno, -1)

    GNL_LOCK_RELEASE(&(q->mtx), -1)

    return 0;
}

/**
 * {@inheritDoc}
 */
void *gnl_ts_nb_queue_dequeue(struct gnl_ts_nb_queue_t *q) {
    GNL_LOCK_ACQUIRE(&(q->mtx), NULL)
    GNL_RELEASE_AND_RETURN_ON_ERROR(q, NULL, &(q->mtx), EINVAL, NULL)

    // remove element
    void *temp = gnl_queue_dequeue(q->q);

    GNL_LOCK_RELEASE(&(q->mtx), NULL)

    return temp;
}

/**
 * {@inheritDoc}
 */
unsigned long gnl_ts_nb_queue_size(struct gnl_ts_nb_queue_t *q) {
    GNL_LOCK_ACQUIRE(&(q->mtx), -1)
    GNL_RELEASE_AND_RETURN_ON_ERROR(q, NULL, &(q->mtx), EINVAL, -1)

    unsigned long temp = q->q->size;

    GNL_LOCK_RELEASE(&(q->mtx), -1)

    return temp;
}

#undef GNL_LOCK_ACQUIRE
#undef GNL_LOCK_RELEASE
#undef GNL_RELEASE_AND_RETURN_ON_ERROR
#include <gnl_macro_end.h>