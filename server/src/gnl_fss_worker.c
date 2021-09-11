#include <unistd.h>
#include <errno.h>
#include <gnl_socket_request.h>
#include <gnl_socket_response.h>
#include <gnl_socket_service.h>
#include <string.h>
#include <gnl_message_n.h>
#include <gnl_list_t.h>
#include <gnl_fss_errno.h> //TODO: eliminare?
#include "../include/gnl_fss_worker.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_fss_worker {

    // the id of the worker
    pthread_t id;

    // the thread-safe blocking bounded queue to use to
    // receive a ready file descriptor from a main thread
    struct gnl_ts_bb_queue_t *worker_queue;

    // the waiting list to store the clients waiting for 
    // a file unlocking
    struct gnl_fss_waiting_list *waiting_list;

    // the pipe channel where to send the result to the
    // main thread
    int pipe_channel;

    // the logger instance to use for logging
    struct gnl_logger *logger;

    // the file system instance to use to store the files
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

static int wait_unlock(struct gnl_simfs_file_system *file_system, struct gnl_fss_waiting_list *waiting_list,
        struct gnl_socket_request *request, int fd_c) {

    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(waiting_list, EINVAL, -1)
    GNL_NULL_CHECK(request, EINVAL, -1)

    char *target;
    int res = 0;
    int fd = -1;

    // get the target
    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            target = request->payload.open->string;
            break;

        case GNL_SOCKET_REQUEST_READ:
            fd = request->payload.read->number;
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            fd = request->payload.write->number;
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            //fd = request->payload.append->number;
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            fd = request->payload.lock->number;
            break;

        default:
            errno = EINVAL;
            res = -1;
            break;
    }

    // if we have only the fd, get the inode to get the "target"
    if (fd >= 0) {
        struct gnl_simfs_inode *inode = NULL;

        // get the inode of the fd
        res = gnl_simfs_file_system_fstat(file_system, fd, inode, fd_c);
        GNL_MINUS1_CHECK(res, errno, -1)
        GNL_NULL_CHECK(inode, errno, -1)

        // get the target
        target = inode->name;
    }

    // put the target and the pid into the waiting list
    return gnl_fss_waiting_list_push(waiting_list, target, fd_c, request);
}

static int get_waiting_fd_c(struct gnl_simfs_file_system *file_system, struct gnl_fss_waiting_list *waiting_list,
        struct gnl_socket_request *request, int fd_c) {

    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(waiting_list, EINVAL, -1)
    GNL_NULL_CHECK(request, EINVAL, -1)

    char *target;
    int res = 0;
    int fd = -1;

    // get the target
    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            target = request->payload.open->string;
            break;

        case GNL_SOCKET_REQUEST_READ:
            fd = request->payload.read->number;
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            fd = request->payload.write->number;
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            //fd = request->payload.append->number;
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            fd = request->payload.lock->number;
            break;

        default:
            errno = EINVAL;
            res = -1;
            break;
    }

    // if we have only the fd, get the inode to get the "target"
    if (fd >= 0) {
        struct gnl_simfs_inode *inode = NULL;

        // get the inode of the fd
        res = gnl_simfs_file_system_fstat(file_system, fd, inode, fd_c);
        GNL_MINUS1_CHECK(res, errno, -1)
        GNL_NULL_CHECK(inode, errno, -1)

        // get the target
        target = inode->name;
    }

    // put the target and the pid into the waiting list
    return gnl_fss_waiting_list_pop(waiting_list, target);
}

/**
 * TODO: doc
 * @param file_system
 * @param request
 * @param fd_c
 * @param waiting_list
 * @return
 */
static struct gnl_socket_response *handle_request(struct gnl_simfs_file_system *file_system,
        struct gnl_socket_request *request, int fd_c) {

    int res;
    struct gnl_socket_response *response = NULL;
    void *buf = NULL;
    size_t count = 0;
    struct gnl_list_t *list = NULL;
    struct gnl_list_t *current = NULL;
    char *tmp;

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
            errno = 0;
            list = gnl_simfs_file_system_ls(file_system, fd_c);

            res = -1;

            // if at least one file is present into the file system
            if (list != NULL) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE_LIST, 0);
                current = list;

                while (current != NULL) {
                    // add the filename to the response
                    tmp = (char *)current->el;
                    res = gnl_socket_response_add_file(response, tmp, (strlen(tmp) + 1), tmp);

                    // if an error occurred, then stop
                    if (res == -1) {
                        gnl_socket_response_destroy(response);
                        break;
                    }

                    // move on to the next element
                    current = current->next;
                }

                gnl_list_destroy(&list, free);
            }
            // else if no file are present (the function returned NULL
            // but no errors occurred, i.e. errno == 0)
            else if (errno == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
                res = 0;
            }
            break;

        case GNL_SOCKET_REQUEST_READ:
            res = gnl_simfs_file_system_read(file_system, request->payload.read->number, &buf, &count, fd_c);

            // if success create an ok_file response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE, 3, "unknown", count, buf);
            }
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
            res = gnl_simfs_file_system_lock(file_system, request->payload.lock->number, fd_c);

            // if success create an ok response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            }
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            res = gnl_simfs_file_system_unlock(file_system, request->payload.unlock->number, fd_c);

            // if success create an ok response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            }
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            res = gnl_simfs_file_system_close(file_system, request->payload.close->number, fd_c);

            // if success create an ok response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            }
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            res = gnl_simfs_file_system_remove(file_system, request->payload.remove->string, fd_c);

            // if success create an ok response
            if (res == 0) {
                response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            }
            break;

        default:
            errno = EINVAL;
            res = -1;
            break;
    }

    // free memory
    free(buf);

    if (res == -1) {
        response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_ERROR, 1, errno);
        GNL_NULL_CHECK(response, errno, NULL)
    }



    // TODO: if this point is reached the response can not be NULL
    //GNL_NULL_CHECK(response, EINVAL, NULL)

    return response;
}

static int send_message_to_master(int pipe_channel, int fd_c) {
    // the message struct to send to the master
    struct gnl_message_n *message_to_master;

    message_to_master = gnl_message_n_init_with_args(fd_c);
    GNL_NULL_CHECK(message_to_master, errno, -1)

    // encode the message
    char *message;
    size_t nwrite = gnl_message_n_to_string(message_to_master, &message);
    GNL_MINUS1_CHECK(nwrite, errno, -1)

    // send the message to the master
    size_t writen = gnl_socket_service_writen(pipe_channel, message, nwrite);
    GNL_MINUS1_CHECK(writen, errno, -1)

    // free memory
    free(message_to_master);
    free(message);

    return 0;
}

static struct gnl_socket_response *handle_fd_c_request(struct gnl_fss_worker *worker, int fd_c, struct gnl_socket_request *request) {
    int res;

    // get the logger
    struct gnl_logger *logger = worker->logger;
    
    // get the request type
    char *request_type;
    res = gnl_socket_request_get_type(request, &request_type);
    GNL_MINUS1_CHECK(res, errno, NULL)

    gnl_logger_debug(logger, "the request has type %s", request_type);

    gnl_logger_debug(logger, "handle the %s request", request_type);

    // the request_type is not necessary anymore, free memory
    free(request_type);

    // handle the request
    struct gnl_socket_response *response;
    response = handle_request(worker->file_system, request, fd_c);

    if (response == NULL) {
        //TODO: creare risposta di errore standard/generica
        gnl_logger_error(logger, "invalid response received from the request handler, stop");

        //TODO: qui c'era la "continue" capire come procedere, magari usando errno per distinguere i casi

        return NULL;
    }

    // if the target file of the request is locked
    if (response->type == GNL_SOCKET_RESPONSE_ERROR && response->payload.error->number == EBUSY) {
        gnl_logger_debug(logger, "EBUSY response received, client %d will be put into the waiting list", fd_c);

        // put the client into the waiting list
        res = wait_unlock(worker->file_system, worker->waiting_list, request, fd_c);
        GNL_MINUS1_CHECK(res, errno, NULL)

        gnl_logger_debug(logger, "client %d successfully put into the waiting list", fd_c);

        // do not send anything to the master
    }
    // else send the response to the client
    else {
        gnl_logger_debug(logger, "client %d request handled", fd_c);

        // encode the response
        char *response_type;
        res = gnl_socket_response_get_type(response, &response_type);
        GNL_MINUS1_CHECK(res, errno, NULL)

        gnl_logger_debug(logger, "building a %s response for client %d", response_type, fd_c);

        free(response_type);

        // send the response message to the client
        gnl_logger_debug(logger, "send the response to client %d", fd_c);

        res = gnl_socket_service_send_response(fd_c, response);
        GNL_MINUS1_CHECK(res, errno, NULL)

        gnl_logger_debug(logger, "response sent to client %d", fd_c);

        // send the message to the master
        send_message_to_master(worker->pipe_channel, fd_c);
    }

    return response;
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_worker *gnl_fss_worker_init(pthread_t id, struct gnl_ts_bb_queue_t *worker_queue,
        struct gnl_fss_waiting_list *waiting_list, int pipe_channel, struct gnl_simfs_file_system *file_system,
                const struct gnl_fss_config *config) {

    // validate parameters
    if (worker_queue == NULL || waiting_list == NULL || file_system == NULL || config == NULL) {
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
    worker->waiting_list = waiting_list;
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

    // generic result var
    int res;

    // the response sent to a client
    struct gnl_socket_response *response;

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
        struct gnl_socket_request *request = gnl_socket_service_get_request(fd_c);

        if (request == NULL) {

            // if EOF...
            if (errno == EPIPE) {

                // close the current file descriptor
                gnl_logger_debug(logger, "the message says that client %d has gone away", fd_c);

                // remove the fd_c from the waiting list (if it was put there)
                res = gnl_fss_waiting_list_remove(worker->waiting_list, fd_c);
                GNL_MINUS1_CHECK(res, errno, NULL)

                //TODO: chiudere tutti i file aperti e unlockarli

                // close the client file descriptor
                res = close(fd_c);
                GNL_MINUS1_CHECK(res, errno, NULL)

                gnl_logger_debug(logger, "closed the connection with client %d", fd_c);

                // send the message to the master, 0 means that a client has gone away
                send_message_to_master(worker->pipe_channel, 0);

            } else {

                gnl_logger_error(logger, "error reading the message: %s, request ignored", strerror(errno));

                // do not stop the server: the show must go on

                // the request is not necessary anymore, destroy it
                gnl_socket_request_destroy(request);

                // resume loop
                continue;
            }
        } else {

            gnl_logger_debug(logger, "the message is a request");

            response = handle_fd_c_request(worker, fd_c, request);
            GNL_NULL_CHECK(response, errno, NULL) //TODO: gestire

            // check for waiting pid
            if (request->type == GNL_SOCKET_REQUEST_UNLOCK && response->type == GNL_SOCKET_RESPONSE_OK) {

                gnl_logger_debug(logger, "broadcast waiting pid");

                int popped_fd_c;
                int broadcast = 0;
                while ((popped_fd_c = get_waiting_fd_c(worker->file_system, worker->waiting_list, request, fd_c)) >= 0) {
                    broadcast++;

                    gnl_logger_debug(logger, "broadcast to pid %d", fd_c);

                    //TODO: qui mi serve la request originale in modo da poterla ripetere,
                    res = handle_fd_c_request(worker, popped_fd_c, request);
                    GNL_MINUS1_CHECK(res, errno, NULL)
                }

                if (broadcast == 0) {
                    gnl_logger_debug(logger, "no waiting pid where present");
                }
             }


            // free memory
            gnl_socket_response_destroy(response);

            // the request is not necessary anymore, destroy it
            gnl_socket_request_destroy(request);
        }


    }

    return NULL;
}

#undef GNL_FSS_WORKER_BUFFER_LEN
#include <gnl_macro_end.h>