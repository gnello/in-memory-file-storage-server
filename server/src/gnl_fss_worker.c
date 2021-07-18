#include <errno.h>
#include <gnl_socket_request.h>
#include "../include/gnl_fss_worker.h"
#include <gnl_macro_beg.h>

#define GNL_FSS_WORKER_BUFFER_LEN 100

/**
 * id               The id of the worker.
 * worker_queue     The queue to use to receive a ready file descriptor
 *                  from the main thread.
 * pipe_channel     The pipe channel where to send the result to the
 *                  main thread.
 * @param logger    The logger instance to use for logging.
 */
struct gnl_fss_worker {
    pthread_t id;
    struct gnl_ts_bb_queue_t *worker_queue;
    int pipe_channel;
    struct gnl_logger *logger;
};

struct gnl_fss_worker *gnl_fss_worker_init(pthread_t id, struct gnl_ts_bb_queue_t *worker_queue, int pipe_channel, const struct gnl_fss_config *config) {
    struct gnl_fss_worker *worker = (struct gnl_fss_worker *)malloc(sizeof(struct gnl_fss_worker));
    GNL_NULL_CHECK(worker, ENOMEM, NULL)

    // instantiate the logger
    struct gnl_logger *logger;

    char channel_name[100];
    snprintf(channel_name, 100, "gnl_fss_worker_%lu", id);

    logger = gnl_logger_init(config->log_filepath, channel_name, config->log_level);
    GNL_NULL_CHECK(logger, errno, NULL)

    worker->logger = logger;

    gnl_logger_debug(worker->logger, "logger created, proceeding initialization");

    // assign the id
    worker->id = id;

    worker->worker_queue = worker_queue;
    worker->pipe_channel = pipe_channel;

    gnl_logger_debug(worker->logger, "initialization completed");

    return worker;
}

void gnl_fss_worker_destroy(struct gnl_fss_worker *worker) {
    if (worker == NULL) {
        return;
    }

    gnl_logger_debug(worker->logger, "destroy requested, proceeding, this is the last message you will see in this channel");

    gnl_logger_destroy(worker->logger);
    free(worker);
}

void *gnl_fss_worker_handle(void* args)
{
    // decode args
    struct gnl_fss_worker *worker = args;

    // get the worker queue
    struct gnl_ts_bb_queue_t *worker_queue = worker->worker_queue;
    int pipe_channel = worker->pipe_channel;
    struct gnl_logger *logger = worker->logger;

    gnl_logger_debug(worker->logger, "ready, waiting for requests");

    // file descriptor of a client read from the queue
    int fd_c;

    // temporary reference to a client file descriptor
    void *raw_fd_c;

    // read buffer
    char buf[GNL_FSS_WORKER_BUFFER_LEN];

    // number of chars read
    long nread;

    // generic result var
    int res;

    // work
    while (1) {

        // waiting for a ready file descriptor from the main thread
        raw_fd_c = gnl_ts_bb_queue_dequeue(worker_queue);
        GNL_NULL_CHECK(raw_fd_c, EINVAL, NULL);

        // cast raw client file descriptor
        fd_c = *(int *)raw_fd_c;

        // if terminate message, put down the worker
        if (fd_c == GNL_FSS_WORKER_TERMINATE) {
            gnl_logger_debug(logger, "termination message received, the thread will be ended");
            break;
        }

        // clear the buffer
        memset(buf, 0, GNL_FSS_WORKER_BUFFER_LEN);

        // read data
        nread = read(fd_c, buf, GNL_FSS_WORKER_BUFFER_LEN);
        GNL_MINUS1_CHECK(nread, errno, NULL)

        // if EOF...
        if (nread == 0) {
            //TODO: create message to tell to the master that a client is gone

            // close the current file descriptor
            gnl_logger_debug(logger, "client %d gone away, closing connection", fd_c);

            // close the client file descriptor
            res = close(fd_c);
            GNL_MINUS1_CHECK(res, errno, NULL)

            // warn the master that a client has gone away TODO: use the gnl_socket_message?
            res = write(worker->pipe_channel, "0", 1);
            GNL_MINUS1_CHECK(res, errno, NULL)
        } else {
            // do something with the buffer...
            gnl_logger_debug(logger, "client %d sent a message, decoding request", fd_c);

            // decode request
            struct gnl_socket_request *request;
            request = gnl_socket_request_read(buf);

            if (request == NULL) {
                gnl_logger_error(logger, "invalid request of client %d: %s", fd_c, strerror(errno));

                // resume loop
                continue;
            }

            char *request_type;
            res = gnl_socket_request_to_string(request, &request_type);
            GNL_MINUS1_CHECK(res, errno, NULL)

            gnl_logger_debug(logger, "client %d sent a request: %s", fd_c, request_type);
        }

        // destroy the exhausted message
        free(raw_fd_c);
    }

    return NULL;
}

#undef GNL_FSS_WORKER_BUFFER_LEN
#include <gnl_macro_end.h>