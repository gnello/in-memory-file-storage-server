#include <unistd.h>
#include <errno.h>
#include <gnl_socket_request.h>
#include <gnl_socket_response.h>
#include <gnl_socket_service.h>
#include <string.h>
#include <gnl_message_n.h>
#include <gnl_fss_errno.h>
#include "../include/gnl_fss_worker.h"
#include <gnl_macro_beg.h>

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
    struct gnl_simfs_file_system *file_system;
};

//static struct gnl_socket_response *get_internal_error_response() {
//    struct gnl_socket_response *response;
//
//    response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_ERROR, 1, GNL_FSS_ERRNO_INTERNAL);
//    GNL_NULL_CHECK(response, errno, NULL)
//
//    return response;
//}
//
//static int throw_internal_error(int fd_c) {
//    int res;
//    struct gnl_socket_response *response;
//
//    // get the internal error response
//    response = get_internal_error_response();
//    GNL_NULL_CHECK(response, errno, -1)
//
//    // encode the response
//    char *response_message;
//    res = gnl_socket_response_write(response, &response_message);
//    GNL_MINUS1_CHECK(res, errno, -1)
//
//    // send the response message to the client //TODO: writen invece di write?
//    res = write(fd_c, response_message, strlen(response_message));
//    GNL_MINUS1_CHECK(res, errno, -1)
//
//    return 0;
//}

static struct gnl_socket_response *handle_request(struct gnl_simfs_file_system *file_system, struct gnl_socket_request *request, int fd_c) {
    int res;
    struct gnl_socket_response *response = NULL;

    // handle the request with the correct handler
    //TODO: creare un metodo handler per ogni type che gestisca separatamente gli errori?
    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            res = gnl_simfs_file_system_open(file_system, request->payload.open->string, request->payload.open->number, fd_c);

            // if success create an ok_fd response
            if (res >= 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FD, 1, res);
            }
            break; //TODO: se "muore" un client chiudere tutti i suoi files aperti (occhio ai lock)

        case GNL_SOCKET_REQUEST_READ_N:
            //res = gnl_simfs_file_system_read_n(file_system, request->payload.read_N->number);
            break;

        case GNL_SOCKET_REQUEST_READ:
            //res = gnl_simfs_file_system_read(file_system, request->payload.read->string);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            res = gnl_simfs_file_system_write(file_system, request->payload.write->number, request->payload.write->bytes, request->payload.write->count, fd_c);

            // if success create an ok response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            }
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            //res = gnl_simfs_file_system_append(file_system, request->payload.append->string, request->payload.append->bytes);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            //res = gnl_simfs_file_system_lock(file_system, request->payload.lock->string);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            //res = gnl_simfs_file_system_unlock(file_system, request->payload.unlock->string);
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            res = gnl_simfs_file_system_close(file_system, request->payload.close->number, fd_c);

            // if success create an ok response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            }
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            //res = gnl_simfs_file_system_remove(file_system, request->payload.remove->string);
            break;

        default:
            errno = EINVAL;
            res = -1;
            break;
    }

    if (res == -1) {
        response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_ERROR, 1, errno);
        GNL_NULL_CHECK(response, errno, NULL)
    }

    // TODO: if this point is reached the response it can not be NULL
    //GNL_NULL_CHECK(response, EINVAL, NULL)

    return response;
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_worker *gnl_fss_worker_init(pthread_t id, struct gnl_ts_bb_queue_t *worker_queue, int pipe_channel,
        struct gnl_simfs_file_system *file_system, const struct gnl_fss_config *config) {
    if (worker_queue == NULL || file_system == NULL || config == NULL) {
        errno = EINVAL;

        return NULL;
    }

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

    // assign the file_system
    worker->file_system = file_system;

    gnl_logger_debug(worker->logger, "initialization completed");

    return worker;
}

/**
 * {@inheritDoc}
 */
void gnl_fss_worker_destroy(struct gnl_fss_worker *worker) {
    if (worker == NULL) {
        return;
    }

    gnl_logger_debug(worker->logger, "destroy requested, proceeding, this is the last message you will see in this channel");

    gnl_logger_destroy(worker->logger);
    free(worker);
}

/**
 * {@inheritDoc}
 */
void *gnl_fss_worker_handle(void* args) {
    // decode args
    struct gnl_fss_worker *worker = args;

    // get the logger
    struct gnl_logger *logger = worker->logger;

    // file descriptor of a client read from the queue
    int fd_c;

    // temporary reference to a client file descriptor
    void *raw_fd_c;

    // number of chars read
    size_t nread;

    // generic result var
    int res;

    // the message struct to send to the master
    struct gnl_message_n *message_to_master;

    gnl_logger_debug(logger, "ready, waiting for requests");

    // work
    while (1) {

        // waiting for a ready file descriptor from the main thread
        raw_fd_c = gnl_ts_bb_queue_dequeue(worker->worker_queue);
        GNL_NULL_CHECK(raw_fd_c, EINVAL, NULL); //TODO: return o si continua il ciclo?

        gnl_logger_debug(logger, "new message received");

        // cast raw client file descriptor
        fd_c = *(int *)raw_fd_c;

        // if terminate message, put down the worker
        if (fd_c == GNL_FSS_WORKER_TERMINATE) {
            gnl_logger_debug(logger, "termination message, the thread will be ended");
            break;
        }

        gnl_logger_debug(logger, "message sent by client %d", fd_c);

        // read data
        struct gnl_socket_request *request = NULL;
        nread = gnl_socket_request_read(fd_c, &request, gnl_socket_service_readn);

        if (nread == -1) {
            gnl_logger_error(logger, "error reading the message: %s", strerror(errno));

            // do not stop the server: the show must go on
            continue;
        }

        // if EOF...
        if (nread == 0) {
            // close the current file descriptor
            gnl_logger_debug(logger, "the message says that client %d has gone away", fd_c);
            //TODO: chiudere tutti i file aperti

            // close the client file descriptor
            res = close(fd_c);
            GNL_MINUS1_CHECK(res, errno, NULL)

            gnl_logger_debug(logger, "closed the connection with client %d", fd_c);

            // create the message for the master, 0 means that a client has gone away
            message_to_master = gnl_message_n_init_with_args(0);
            GNL_NULL_CHECK(message_to_master, errno, NULL)
        } else {

            gnl_logger_debug(logger, "the message is a request");

            if (request == NULL) {
                gnl_logger_error(logger, "invalid request: %s", strerror(errno));
            } else {

                // get the request type
                char *request_type;
                res = gnl_socket_request_to_string(request, &request_type);
                GNL_MINUS1_CHECK(res, errno, NULL)

                gnl_logger_debug(logger, "request decoded, has type %s", request_type);

                gnl_logger_debug(logger, "handle the %s request", request_type);

                // the request_type is not necessary anymore, free memory
                free(request_type);

                // handle the request
                struct gnl_socket_response *response;
                response = handle_request(worker->file_system, request, fd_c);

                if (response == NULL) {
                    //TODO: creare risposta di errore standard/generica
                    gnl_logger_error(logger, "invalid response received from the request handler, stop");

                    continue;
                }

                gnl_logger_debug(logger, "client %d request handled", fd_c);

                // encode the response
                char *response_type;
                res = gnl_socket_response_to_string(response, &response_type);
                GNL_MINUS1_CHECK(res, errno, NULL)

                gnl_logger_debug(logger, "building a %s response for client %d", response_type, fd_c);

                free(response_type);

                char *response_message = NULL;
                res = gnl_socket_response_write(response, &response_message);
                GNL_MINUS1_CHECK(res, errno, NULL) // TODO: scrivere log!!!!

                // send the response message to the client //TODO: writen invece di write?
                gnl_logger_debug(logger, "send a response to client %d", fd_c);

                res = write(fd_c, response_message, strlen(response_message)); //TODO usare il socket service che implementa writen
                GNL_MINUS1_CHECK(res, errno, NULL)

                gnl_logger_debug(logger, "response sent to client %d", fd_c);

                // free memory
                free(response_message);
                gnl_socket_response_destroy(response);
            }

            // the request is not necessary anymore, destroy it
            gnl_socket_request_destroy(request);

            // create the message for the master
            message_to_master = gnl_message_n_init_with_args(fd_c);
            GNL_NULL_CHECK(message_to_master, errno, NULL)
        }

        // encode the message
        char *message;
        size_t nwrite = gnl_message_n_to_string(message_to_master, &message);
        GNL_MINUS1_CHECK(nwrite, errno, NULL)

        // send the message to the master
        res = write(worker->pipe_channel, message, nwrite); //TODO: usare writen oppure il socket service
        GNL_MINUS1_CHECK(res, errno, NULL)

        // free memory
        free(message_to_master);
        free(message);
    }

    return NULL;
}

#undef GNL_FSS_WORKER_BUFFER_LEN
#include <gnl_macro_end.h>