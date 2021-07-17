
#include <stdlib.h>
#include <pthread.h>
#include <gnl_ts_bb_queue_t.h>
#include "./gnl_fss_worker.c"
#include "../include/gnl_fss_thread_pool.h"
#include <gnl_macro_beg.h>

/**
 * worker_ids       The array of the thread pool workers identifiers.
 * worker_config    The config for a single worker.
 * worker_queue     The queue to use to receive a ready file descriptor
 *                  from a main thread.
 * pipe             The pipe channel where to send the result to a
 *                  main thread.
 * size             The size of the thread pool.
 */
struct gnl_fss_thread_pool {
    pthread_t *worker_ids;
    struct gnl_fss_worker_config *worker_config;
    struct gnl_ts_bb_queue_t *worker_queue;
    int pipe_channel;
    int size;
};

struct gnl_fss_thread_pool *gnl_fss_thread_pool_init(int size) {
    struct gnl_fss_thread_pool *thread_pool = (struct gnl_fss_thread_pool *)malloc(sizeof(struct gnl_fss_thread_pool));
    GNL_NULL_CHECK(thread_pool, ENOMEM, NULL)

    // allocate memory for the worker ids
    thread_pool->worker_ids = malloc(size * sizeof(pthread_t));
    GNL_NULL_CHECK(thread_pool->worker_ids, ENOMEM, NULL)

    // instantiate the blocking bounded queue to communicate with the workers
    thread_pool->worker_queue = gnl_ts_bb_queue_init(size);
    GNL_NULL_CHECK(thread_pool->worker_queue, errno, NULL)

    // create the pipe channel
    thread_pool->pipe_channel = 1;

    int res;

    // instantiate the thread pool
    thread_pool->worker_config = gnl_fss_worker_init(thread_pool->worker_queue, thread_pool->pipe_channel);
    GNL_NULL_CHECK(thread_pool->worker_config, errno, NULL)

    for (size_t i=0; i<size; i++) {
        res = pthread_create(&(thread_pool->worker_ids[i]), NULL, &gnl_fss_worker_handle, (void *)thread_pool->worker_config);
        if (res != 0) {
            return NULL;
        }
    }

    // add the size
    thread_pool->size = size;

    return thread_pool;
}

void gnl_fss_thread_pool_destroy(struct gnl_fss_thread_pool *thread_pool) {
    // destroy the worker config
    gnl_fss_worker_destroy(thread_pool->worker_config);

    // send one termination message per thread into the pool worker
    int termination_mex = GNL_FSS_WORKER_TERMINATE;
    for (size_t i=0; i<thread_pool->size; i++) {
        gnl_ts_bb_queue_enqueue(thread_pool->worker_queue, (void *)&termination_mex);
    }

    // wait for all threads to die
    for (size_t i=0; i<thread_pool->size; i++) {
        pthread_join(thread_pool->worker_ids[i], NULL);
    }

    //TODO: do the same with the logger thread

    // destroy the worker ids
    free(thread_pool->worker_ids);

    // destroy the worker queue
    gnl_ts_bb_queue_destroy(thread_pool->worker_queue, NULL);

    // destroy the thread pool
    free(thread_pool);
}

#include <gnl_macro_end.h>