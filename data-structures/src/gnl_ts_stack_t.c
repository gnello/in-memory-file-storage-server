/*
 * This is a simple concurrent thead safe stack implementation, it does not intend
 * to be exhaustive but it's nice :)
 */

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "gnl_stack_t.c"
#include "../include/gnl_ts_stack_t.h"
#include <gnl_macro_beg.h>

struct gnl_ts_stack_t {
    pthread_mutex_t mtx;
    gnl_stack_t *s;
};

gnl_ts_stack_t *gnl_ts_stack_init() {
    gnl_ts_stack_t *stack = (gnl_ts_stack_t*)malloc(sizeof(gnl_ts_stack_t));

    GNL_NULL_CHECK(stack, ENOMEM, NULL)

    int pthread_res = pthread_mutex_init(&(stack->mtx), NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    stack->s = gnl_stack_init();

    GNL_NULL_CHECK(stack->s, ENOMEM, NULL)

    return stack;
}

int gnl_ts_stack_destroy(gnl_ts_stack_t *s, void (*destroy)(void *data)) {
    int pthread_res;
    pthread_mutex_t mtx;

    pthread_res = pthread_mutex_init(&mtx, NULL);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    pthread_res = pthread_mutex_lock(&mtx);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    if (s == NULL) {
        return 0;
    }

    gnl_stack_destroy((s->s), destroy);

    pthread_res = pthread_mutex_destroy(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    free(s);

    pthread_res = pthread_mutex_unlock(&mtx);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    pthread_res = pthread_mutex_destroy(&mtx);
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    return 0;
}

int gnl_ts_stack_push(gnl_ts_stack_t *s, void *el) {
    GNL_NULL_CHECK(s, EINVAL, -1)

    int pthread_res;

    pthread_res = pthread_mutex_lock(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    int res = gnl_stack_push(s->s, el);
    GNL_MINUS1_CHECK(res, errno, -1) //TODO: rilsciare il lock!

    pthread_res = pthread_mutex_unlock(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    return 0;
}

void *gnl_ts_stack_pop(gnl_ts_stack_t *s) {
    GNL_NULL_CHECK(s, EINVAL, NULL)

    int pthread_res;

    if (gnl_ts_stack_size(s) == 0) { //TODO: spostare dentro il lock
        return NULL; //TODO: rimuovere il lock
    }

    pthread_res = pthread_mutex_lock(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    void *temp = gnl_stack_pop(s->s);

    pthread_res = pthread_mutex_unlock(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, NULL)

    return temp;
}

unsigned long gnl_ts_stack_size(gnl_ts_stack_t *s) {
    GNL_NULL_CHECK(s, EINVAL, -1)

    int pthread_res;

    pthread_res = pthread_mutex_lock(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    unsigned long temp = s->s->size;

    pthread_res = pthread_mutex_unlock(&(s->mtx));
    GNL_MINUS1_CHECK(pthread_res, errno, -1)

    return temp;
}

#include <gnl_macro_end.h>