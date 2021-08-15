
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <gnl_ts_bb_queue_t.h>
#include "./gnl_fss_worker.c"
#include "../include/gnl_fss_thread_pool.h"
#include <gnl_macro_beg.h>

/**
 * worker_ids           The array of the thread pool workers identifiers.
 * worker               The array of workers instances.
 * worker_queue         The queue to use to receive a ready file descriptor
 *                      from a main thread.
 * file_system          The file system instance to use to store the files.
 * pipe_master_channel  The pipe channel where to read a result from a
 *                      worker thread.
 * pipe_worker_channel  The pipe channel where to send the result to a
 *                      master thread.
 * size                 The size of the thread pool.
 * logger               The logger instance to use for logging.
 */
struct gnl_fss_thread_pool {
    pthread_t *worker_ids;
    struct gnl_fss_worker **workers;
    struct gnl_ts_bb_queue_t *worker_queue;
    struct gnl_simfs_file_system *file_system;
    int pipe_master_channel;
    int pipe_worker_channel;
    int size;
    struct gnl_logger *logger;
};

struct gnl_fss_thread_pool *gnl_fss_thread_pool_init(int size, struct gnl_simfs_file_system *file_system,
        const struct gnl_fss_config *config) {
    if (file_system == NULL || config == NULL) {
        errno = EINVAL;

        return NULL;
    }

    int res;

    // instantiate the thread pool struct
    struct gnl_fss_thread_pool *thread_pool = (struct gnl_fss_thread_pool *)malloc(sizeof(struct gnl_fss_thread_pool));
    GNL_NULL_CHECK(thread_pool, ENOMEM, NULL)

    // instantiate the logger
    struct gnl_logger *logger;
    logger = gnl_logger_init(config->log_filepath, "gnl_fss_thread_pool", config->log_level);
    GNL_NULL_CHECK(logger, errno, NULL)

    thread_pool->logger = logger;

    gnl_logger_debug(thread_pool->logger, "logger created, proceeding initialization");

    // assign the file_system
    thread_pool->file_system = file_system;

    // allocate memory for the worker ids
    thread_pool->worker_ids = malloc(size * sizeof(pthread_t));
    GNL_NULL_CHECK(thread_pool->worker_ids, ENOMEM, NULL)

    // instantiate the blocking bounded queue to communicate with the workers
    thread_pool->worker_queue = gnl_ts_bb_queue_init(size);
    GNL_NULL_CHECK(thread_pool->worker_queue, errno, NULL)

    gnl_logger_debug(thread_pool->logger, "workers blocking bounded queue created");

    // create the pipe channels
    int pipe_channels[2];

    res = pipe(pipe_channels);
    GNL_MINUS1_CHECK(res, errno, NULL)

    thread_pool->pipe_master_channel = pipe_channels[0];
    thread_pool->pipe_worker_channel = pipe_channels[1];

    gnl_logger_debug(thread_pool->logger, "workers pipe channels created");

    // instantiate the thread pool workers
    thread_pool->workers = (struct gnl_fss_worker **) malloc(size * sizeof(struct gnl_fss_worker *));
    GNL_NULL_CHECK(thread_pool->workers, ENOMEM, NULL)

    gnl_logger_debug(thread_pool->logger, "starting %d threads", size);

    for (size_t i=0; i<size; i++) {
        thread_pool->workers[i] = gnl_fss_worker_init(i, thread_pool->worker_queue, thread_pool->pipe_worker_channel,
                                                      thread_pool->file_system, config);
        GNL_NULL_CHECK(thread_pool->workers[i], errno, NULL)

        res = pthread_create(&(thread_pool->worker_ids[i]), NULL, &gnl_fss_worker_handle, (void *)thread_pool->workers[i]);
        if (res != 0) {
            gnl_logger_warn(thread_pool->logger, "error starting a thread: %s", strerror(errno));

            return NULL;
        }
    }

    gnl_logger_debug(thread_pool->logger, "%d threads started", size);

    // add the size of the thread pool
    thread_pool->size = size;

    gnl_logger_debug(thread_pool->logger, "initialization completed");

    return thread_pool;
}

void gnl_fss_thread_pool_destroy(struct gnl_fss_thread_pool *thread_pool) {
    if (thread_pool == NULL) {
        return;
    }

    int res;

    gnl_logger_debug(thread_pool->logger, "destroy requested, proceeding");

    // send one termination message per worker into the thread pool
    gnl_logger_debug(thread_pool->logger, "sending a termination message to %d threads", thread_pool->size);

    int termination_mex = GNL_FSS_WORKER_TERMINATE;
    int count = 0;
    for (size_t i=0; i<thread_pool->size; i++) {
        res = gnl_fss_thread_pool_dispatch(thread_pool, (void *)&termination_mex);

        if (res != -1) {
            count++;
        }
    }

    gnl_logger_debug(thread_pool->logger, "sent %d termination messages of %d required", count, thread_pool->size);

    // wait for all worker to end
    for (size_t i=0; i<thread_pool->size; i++) {
        pthread_join(thread_pool->worker_ids[i], NULL);

        // destroy worker instance
        gnl_fss_worker_destroy(thread_pool->workers[i]);
    }

    free(thread_pool->workers);

    gnl_logger_debug(thread_pool->logger, "ended %d threads", thread_pool->size);

    // destroy the worker ids
    free(thread_pool->worker_ids);

    // close the pipe channels
    close(thread_pool->pipe_master_channel);
    close(thread_pool->pipe_worker_channel);

    // destroy the worker queue
    gnl_ts_bb_queue_destroy(thread_pool->worker_queue, NULL);

    gnl_logger_debug(thread_pool->logger, "destroy almost finished, this is the last message you will see in this channel");

    // destroy the logger
    gnl_logger_destroy(thread_pool->logger);

    // destroy the thread pool
    free(thread_pool);
}

int gnl_fss_thread_pool_dispatch(struct gnl_fss_thread_pool *thread_pool, void *message) {
    GNL_NULL_CHECK(thread_pool, EINVAL, -1)

    gnl_logger_debug(thread_pool->logger, "dispatching a message to the thread pool");

    return gnl_ts_bb_queue_enqueue(thread_pool->worker_queue, message);
}

int gnl_fss_thread_pool_master_channel(struct gnl_fss_thread_pool *thread_pool) {
    GNL_NULL_CHECK(thread_pool, EINVAL, -1)

    return thread_pool->pipe_master_channel;
}

#include <gnl_macro_end.h>