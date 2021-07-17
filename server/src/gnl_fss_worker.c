#include <errno.h>
#include "../include/gnl_fss_worker.h"
#include <gnl_macro_beg.h>

/**
 * worker_queue The queue to use to receive a ready file descriptor
 *              from the main thread.
 * pipe         The pipe channel where to send the result to the
 *              main thread.
 */
struct gnl_fss_worker_config {
    struct gnl_ts_bb_queue_t *worker_queue;
    int pipe_channel;
};

struct gnl_fss_worker_config *gnl_fss_worker_init(struct gnl_ts_bb_queue_t *worker_queue, int pipe_channel) {
    struct gnl_fss_worker_config *worker_config = (struct gnl_fss_worker_config *)malloc(sizeof(struct gnl_fss_worker_config));
    GNL_NULL_CHECK(worker_config, ENOMEM, NULL)

    worker_config->worker_queue = worker_queue;
    worker_config->pipe_channel = pipe_channel;

    return worker_config;
}

void gnl_fss_worker_destroy(struct gnl_fss_worker_config *worker_config) {
    free(worker_config);
}

void *gnl_fss_worker_handle(void* args)
{
    // decode args
    struct gnl_fss_worker_config *worker_config = args;

    // get the worker queue
    struct gnl_ts_bb_queue_t *worker_queue = worker_config->worker_queue;
    int pipe_channel = ((struct gnl_fss_worker_config*)args)->pipe_channel;

    // file descriptor of a client read from the queue
    int fd_c;

    // temporary reference to a client file descriptor
    void *raw_fd_c;

    // work
    while (1) {

        // waiting for a ready file descriptor for the main thread
        raw_fd_c = gnl_ts_bb_queue_dequeue(worker_queue);
        GNL_NULL_CHECK(raw_fd_c, EINVAL, NULL);

        // cast raw client file descriptor
        fd_c = *(int *)raw_fd_c;

        // if terminate message, put down the worker
        if (fd_c == GNL_FSS_WORKER_TERMINATE) {
            break;
        }
    }

    return NULL;
}

#include <gnl_macro_end.h>