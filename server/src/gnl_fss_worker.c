#include <errno.h>
#include <gnl_socket_request.h>
#include <gnl_message_n.h>
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

    // get the logger
    struct gnl_logger *logger = worker->logger;

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

    // the message struct to send to the master
    struct gnl_message_n *message_to_master;

    gnl_logger_debug(logger, "ready, waiting for requests");

    // work
    while (1) {

        // waiting for a ready file descriptor from the main thread
        raw_fd_c = gnl_ts_bb_queue_dequeue(worker->worker_queue);
        GNL_NULL_CHECK(raw_fd_c, EINVAL, NULL);

        gnl_logger_debug(logger, "received a new message");

        // cast raw client file descriptor
        fd_c = *(int *)raw_fd_c;

        // if terminate message, put down the worker
        if (fd_c == GNL_FSS_WORKER_TERMINATE) {
            gnl_logger_debug(logger, "termination message, the thread will be ended");
            break;
        }

        gnl_logger_debug(logger, "message sent by client %d", fd_c);

        // clear the buffer
        memset(buf, 0, GNL_FSS_WORKER_BUFFER_LEN);

        // read data
        nread = read(fd_c, buf, GNL_FSS_WORKER_BUFFER_LEN);
        if (nread == -1) {
            gnl_logger_error(logger, "error reading the message: %s", strerror(errno));

            // do not stop the server: the show must go on
            continue;
        }

        // if EOF...
        if (nread == 0) {
            // close the current file descriptor
            gnl_logger_debug(logger, "the message says that client %d has gone away", fd_c);

            // close the client file descriptor
            res = close(fd_c);
            GNL_MINUS1_CHECK(res, errno, NULL)

            gnl_logger_debug(logger, "closed the connection with client %d", fd_c);

            // create the message for the master, 0 means that a client has gone away
            message_to_master = gnl_message_n_init_with_args(0);
            GNL_NULL_CHECK(message_to_master, errno, NULL)
        } else {

            gnl_logger_debug(logger, "the message is a request, decoding", fd_c);

            // decode request
            struct gnl_socket_request *request;
            request = gnl_socket_request_read(buf);

            if (request == NULL) {
                gnl_logger_error(logger, "invalid request: %s", strerror(errno));
            } else {

                char *request_type;
                res = gnl_socket_request_to_string(request, &request_type);
                GNL_MINUS1_CHECK(res, errno, NULL)

                gnl_logger_debug(logger, "request decoded, has type %s", request_type);

                // the request_type is not necessary anymore, free memory
                free(request_type);
            }

            // the request is not necessary anymore, destroy it
            gnl_socket_request_destroy(request);

            // create the message for the master
            message_to_master = gnl_message_n_init_with_args(fd_c);
            GNL_NULL_CHECK(message_to_master, errno, NULL)
        }

        // encode the message
        char *message;
        res = gnl_message_n_write(*message_to_master, &message);
        GNL_MINUS1_CHECK(res, errno, NULL)

        // send the message to the master
        res = write(worker->pipe_channel, message, strlen(message));
        GNL_MINUS1_CHECK(res, errno, NULL)

        // free memory
        free(message_to_master);
        free(message);
    }

    return NULL;
}

#undef GNL_FSS_WORKER_BUFFER_LEN
#include <gnl_macro_end.h>