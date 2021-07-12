/*
 * This is a simple concurrent thead safe blocking bounded stack implementation, it does not intend
 * to be exhaustive but it's nice :)
 */

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "gnl_stack_t.c"
#include "../include/gnl_ts_bb_stack_t.h"
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
 * mtx          The mutex var of the thread-safe blocking bounded stack.
 * el_added     The cond var to track if an element is added.
 * el_removed   The cond var to track if an element is removed.
 * s            The stack data-structure.
 * bound        The max number of items storable in the stack.
 */
struct gnl_ts_bb_stack_t {
    pthread_mutex_t mtx;
    pthread_cond_t el_added;
    pthread_cond_t el_removed;
    struct gnl_stack_t *s;
    int bound;
};

struct gnl_ts_bb_stack_t *gnl_ts_bb_stack_init(int bound) {
    if (bound <= 0) {
        errno = EINVAL;

        return NULL;
    }

    struct gnl_ts_bb_stack_t *stack = (struct gnl_ts_bb_stack_t*)malloc(sizeof(struct gnl_ts_bb_stack_t));
    GNL_NULL_CHECK(stack, ENOMEM, NULL)

    // initialize lock
    int pthread_res = pthread_mutex_init(&(stack->mtx), NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    // initialize condition variables
    pthread_res = pthread_cond_init(&(stack->el_added), NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    pthread_res = pthread_cond_init(&(stack->el_removed), NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    // initialize stack
    stack->s = gnl_stack_init();
    GNL_NULL_CHECK(stack->s, ENOMEM, NULL)

    // set the bound
    stack->bound = bound;

    return stack;
}

int gnl_ts_bb_stack_destroy(struct gnl_ts_bb_stack_t *s, void (*destroy)(void *data)) {
    int res;

    if (s == NULL) {
        return 0;
    }

    gnl_stack_destroy((s->s), destroy);

    res = pthread_mutex_destroy(&(s->mtx));
    GNL_MINUS1_CHECK(res, errno, -1)

    res = pthread_cond_destroy(&(s->el_added));
    GNL_MINUS1_CHECK(res, errno, -1)

    res = pthread_cond_destroy(&(s->el_removed));
    GNL_MINUS1_CHECK(res, errno, -1)

    free(s);

    return 0;
}

int gnl_ts_bb_stack_push(struct gnl_ts_bb_stack_t *s, void *el) {
    int res;

    GNL_LOCK_ACQUIRE(&(s->mtx), -1)
    GNL_RELEASE_AND_RETURN_ON_ERROR(s, NULL, &(s->mtx), EINVAL, -1)

    // wait until there is a room
    while (gnl_stack_size(s->s) == s->bound) {
        res = pthread_cond_wait(&(s->el_removed), &(s->mtx));
        GNL_RELEASE_AND_RETURN_ON_ERROR(res, -1, &(s->mtx), errno, -1)
    }

    // insert element
    res = gnl_stack_push(s->s, el);
    GNL_RELEASE_AND_RETURN_ON_ERROR(res, -1, &(s->mtx), errno, -1)

    // wake up eventually waiting threads
    res = pthread_cond_signal(&(s->el_added));
    GNL_RELEASE_AND_RETURN_ON_ERROR(res, -1, &(s->mtx), errno, -1)

    GNL_LOCK_RELEASE(&(s->mtx), -1)

    return 0;
}

void *gnl_ts_bb_stack_pop(struct gnl_ts_bb_stack_t *s) {
    int res;

    GNL_LOCK_ACQUIRE(&(s->mtx), NULL)
    GNL_RELEASE_AND_RETURN_ON_ERROR(s, NULL, &(s->mtx), EINVAL, NULL)

    // wait until there is an element
    while (gnl_stack_size(s->s) == 0) {
        res = pthread_cond_wait(&(s->el_added), &(s->mtx));
        GNL_RELEASE_AND_RETURN_ON_ERROR(res, -1, &(s->mtx), errno, NULL)
    }

    // remove element
    void *temp = gnl_stack_pop(s->s);

    // wake up eventually waiting threads
    res = pthread_cond_signal(&(s->el_removed));
    GNL_RELEASE_AND_RETURN_ON_ERROR(res, -1, &(s->mtx), errno, NULL)

    GNL_LOCK_RELEASE(&(s->mtx), NULL)

    return temp;
}

unsigned long gnl_ts_bb_stack_size(struct gnl_ts_bb_stack_t *s) {
    GNL_LOCK_ACQUIRE(&(s->mtx), -1)
    GNL_RELEASE_AND_RETURN_ON_ERROR(s, NULL, &(s->mtx), EINVAL, -1)

    unsigned long temp = s->s->size;

    GNL_LOCK_RELEASE(&(s->mtx), -1)

    return temp;
}

#undef GNL_LOCK_ACQUIRE
#undef GNL_LOCK_RELEASE
#undef GNL_RELEASE_AND_RETURN_ON_ERROR
#include <gnl_macro_end.h>