/*
 * This is a simple concurrent thead safe stack implementation, it does not intend
 * to be exhaustive but it's nice :)
 */

#include <stdlib.h>
#include <pthread.h>
#include "gnl_stack_t.c"
#include "../includes/gnl_ts_stack_t.h"

struct gnl_ts_stack_t {
    pthread_mutex_t mtx;
    gnl_stack_t *s;
};

gnl_ts_stack_t *gnl_ts_stack_init() {
    gnl_ts_stack_t *stack = (gnl_ts_stack_t*)malloc(sizeof(gnl_ts_stack_t));
    if (stack == NULL) {
        perror("malloc");

        return NULL;
    }

    int pthread_res = pthread_mutex_init(&(stack->mtx), NULL);
    if (pthread_res == -1) {
        perror("pthread_mutex_init");

        return NULL;
    }

    stack->s = gnl_stack_init();
    if (stack->s == NULL) {
        perror("gnl_stack_init");

        return NULL;
    }

    return stack;
}

int gnl_ts_stack_destroy(gnl_ts_stack_t *s) {
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

    if (s == NULL) {
        return 0;
    }

    gnl_stack_destroy((s->s));

    pthread_res = pthread_mutex_destroy(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_destroy");

        return -1;
    }

    free(s);

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

int gnl_ts_stack_push(gnl_ts_stack_t *s, void *el) {
    int pthread_res;

    pthread_res = pthread_mutex_lock(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return -1;
    }

    int res = gnl_stack_push(s->s, el);
    if (res == -1) {
        perror("gnl_stack_push");

        return -1;
    }

    pthread_res = pthread_mutex_unlock(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return 0;
}

void *gnl_ts_stack_pop(gnl_ts_stack_t *s) {
    int pthread_res;

    if (gnl_ts_stack_size(s) == 0) {
        return NULL;
    }

    pthread_res = pthread_mutex_lock(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return NULL;
    }

    void *temp = gnl_stack_pop(s->s);

    pthread_res = pthread_mutex_unlock(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return NULL;
    }

    return temp;
}

unsigned long gnl_ts_stack_size(gnl_ts_stack_t *s) {
    int pthread_res;

    pthread_res = pthread_mutex_lock(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_lock");

        return -1;
    }

    unsigned long temp = s->s->size;

    pthread_res = pthread_mutex_unlock(&(s->mtx));
    if (pthread_res == -1) {
        perror("pthread_mutex_unlock");

        return -1;
    }

    return temp;
}