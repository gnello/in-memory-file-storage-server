#include <time.h>
#include <errno.h>
#include <string.h>
#include <gnl_file_saver.h>
#include <gnl_socket_request.h>
#include <gnl_socket_response.h>
#include <gnl_socket_service.h>
#include <gnl_file_to_pointer.h>
#include <gnl_ternary_search_tree_t.h>
#include "../include/gnl_fss_api.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
static struct gnl_socket_connection *socket_service_connection;

/**
 * Whether the socket connection is active or not.
 */
int socket_service_connection_active = 0;

/**
 * Whether the openFile(pathname, O_CREATE|O_LOCK) api was called.
 * Is set to 1 by the openFile method if the required flags are met,
 * all other methods set it to 0.
 * It used by the writeFile api to check if she can write a whole file
 * from scratch.
 */
int open_with_create_lock_flags = 0;

/**
 * The file descriptor table that holds all the open file descriptors
 * returned by the openFile api.
 */
struct gnl_ternary_search_tree_t *file_descriptor_table;

/**
 * Send the given request to the server and get the response.
 * A call to this invocation will destroy the given request.
 *
 * @param request   The request to send.
 *
 * @return          Returns the response from the server on success,
 *                  NULL otherwise.
 */
static struct gnl_socket_response *send_and_destroy_request(struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, errno, NULL)

    // send the request to the server
    int bytes_sent = gnl_socket_service_send_request(socket_service_connection, request);
    GNL_MINUS1_CHECK(bytes_sent, errno, NULL)

    // clean memory
    gnl_socket_request_destroy(request);

    // get the response from the server
    return gnl_socket_service_get_response(socket_service_connection);
}

/**
 * {@inheritDoc}
 */
int openConnection(const char *sockname, int msec, const struct timespec abstime) {
    if (sockname == NULL || msec <= 0) {
        errno = EINVAL;

        return -1;
    }

    // maintain only 1 connection active to the server
    if (socket_service_connection_active) {
        errno = EINVAL;

        return -1;
    }

    // try to connect to the given socket name
    while ((socket_service_connection = gnl_socket_service_connect(sockname)) == NULL) {
        if (errno != ENOENT) {
            return -1;
        }

        time_t now = time(NULL);

        // max wait time reached, return
        if (now > abstime.tv_sec) {
            // let the errno bubble

            return -1;
        }

        // wait the given amount of time
        struct timespec tim;

        if(msec > 999) {
            tim.tv_sec = (int)(msec / 1000);
            tim.tv_nsec = (msec - ((long)tim.tv_sec * 1000)) * 1000000;
        } else {
            tim.tv_sec = 0;
            tim.tv_nsec = msec * 1000000;
        }

        nanosleep(&tim, NULL);
    }

    // initialize the file descriptor table
    file_descriptor_table = NULL;

    // set the connection active
    socket_service_connection_active = 1;

    // reset the openFile(pathname, O_CREATE|O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int closeConnection(const char *sockname) {
    // validate the state
    if (!socket_service_connection_active
    || !gnl_socket_service_is_active(socket_service_connection)
    || socket_service_connection->socket_name == NULL
    || strcmp(socket_service_connection->socket_name, sockname) != 0) {
        errno = EINVAL;

        return -1;
    }

    int res = gnl_socket_service_close(socket_service_connection);
    if (res != -1) {
        socket_service_connection_active = 0;
    }

    // destroy the file descriptor table
    gnl_ternary_search_tree_destroy(&file_descriptor_table, NULL);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    // all the open file will be close by the server as soon
    // as he received the "close connection" socket message

    return res;
}

/**
 * {@inheritDoc}
 */
int openFile(const char *pathname, int flags) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, pathname, flags);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;
    int tmp_res;
    unsigned int *fd_copy = NULL;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK_FD:
            // if success, appropriately set the open_with_create_lock_flags
            open_with_create_lock_flags = flags & (O_CREATE | O_LOCK);

            // make a deep copy of the fd
            fd_copy = malloc(sizeof(int));
            GNL_NULL_CHECK(fd_copy, ENOMEM, -1)

            // get the file descriptor from the response
            *fd_copy = gnl_socket_response_get_fd(response);
            GNL_MINUS1_CHECK(*fd_copy, errno, -1)

            // add the deep copy into the file_descriptor_table
            tmp_res = gnl_ternary_search_tree_put(&file_descriptor_table, pathname, fd_copy);
            GNL_MINUS1_CHECK(tmp_res, errno, -1)

            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    return res;
}

/**
 * {@inheritDoc}
 */
int readFile(const char *pathname, void **buf, size_t *size) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the fd bound to the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_READ, 1, fd);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)
            
            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE:
            // get the size
            *size = gnl_socket_response_get_size(response);

            // instantiate the buf
            GNL_CALLOC(*buf, *size, -1)

            // copy the received bytes into buf
            memcpy(*buf, gnl_socket_response_get_bytes(response), *size);
            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
int readNFiles(int N, const char *dirname) {
    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_READ_N, 1, N);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;
    struct gnl_message_snb *file = NULL;

    // counter of the read files
    int file_read_count = 0;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // no need to do something else here
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE_LIST:

            // for each received file
            while ((file = gnl_socket_response_get_file(response)) != NULL) {

                // increase the counter
                file_read_count++;

                // start reading the file
                char *filename = file->string;

                // open the file on the server
                res = openFile(filename, 0);
                if (res == -1) {
                    // let the errno bubble
                    break;
                }

                // read the file
                void *buf = NULL;
                size_t size;

                int res_read = readFile(filename, &buf, &size);
                int errno_read = errno;

                // an eventual error during the read will be checked later

                // close the file
                int res_close = closeFile(filename);
                int errno_close = errno;

                // check if there was an error during the read
                if (res_read == -1) {
                    errno = errno_read;
                    break;
                }

                // check if there was an error during the close
                if (res_close == -1) {
                    errno = errno_close;
                    break;
                }

                // store the read file on disk
                if (dirname != NULL) {
                    res = gnl_file_saver_save(filename, dirname, buf, size);

                    if (res == -1) {
                        // let the errno bubble
                        break;
                    }
                }

                //free memory
                free(buf);
                gnl_message_snb_destroy(file);

                // check if we have to stop; we do the check here
                // since at least 1 file must be read (N > 0) or
                // all the files must be read (N <= 0), in addition
                // at this point all the memory was freed
                if (N > 0 && file_read_count == N) {
                    break;
                }
            }
            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if there was an error, then stop
    if (res == -1) {
        return -1;
    }

    // if success, then reset the openFile(pathname, O_CREATE|O_LOCK) check
    open_with_create_lock_flags = 0;

    // return the number of files read
    return file_read_count;
}

/**
 * {@inheritDoc}
 */
int writeFile(const char *pathname, const char *dirname) {
    // check if the previous call was to the openFile
    // with the O_CREATE|O_LOCK flags
    if (open_with_create_lock_flags == 0) {
        errno = EPERM;
        return -1;
    }

    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the file to send
    long size;
    char *file = NULL;

    int res = gnl_file_to_pointer(pathname, &file, &size);

    GNL_MINUS1_CHECK(res, errno, -1)

    // send the file through the "append" call
    res = appendToFile(pathname, file, size, dirname);

    // free memory
    free(file);

    return res;
}

/**
 * {@inheritDoc}
 */
int appendToFile(const char *pathname, void *buf, size_t size, const char *dirname) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the fd bound to the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_WRITE, 3, fd, size, buf);
    GNL_NULL_CHECK(request, errno, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);

    // check the response
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;
    struct gnl_message_snb *file = NULL;

    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // no need to do something else here
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE_LIST:
            // success but one or more files were evicted

            // for each received file
            while ((file = gnl_socket_response_get_file(response)) != NULL) {

                // if a dirname was provided, then save the file
                if (dirname != NULL) {
                    res = gnl_file_saver_save(file->string, dirname, file->bytes, file->count);

                    if (res == -1) {
                        // let the errno bubble
                        break;
                    }
                }

                gnl_message_snb_destroy(file);
            }
            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
            break;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
int lockFile(const char *pathname) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the fd bound to the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_LOCK, 1, fd);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // no need to do something else here
            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
int unlockFile(const char *pathname) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the fd bound to the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_UNLOCK, 1, fd);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // no need to do something else here
            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
int closeFile(const char *pathname) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the fd bound to the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_CLOSE, 1, fd);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;
    int tmp_res;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // remove the fd from the file_descriptor_table
            tmp_res = gnl_ternary_search_tree_remove(file_descriptor_table, pathname, free);
            GNL_MINUS1_CHECK(tmp_res, errno, -1)

            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
int removeFile(const char *pathname) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_REMOVE, 1, pathname);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request and get the response from the server
    struct gnl_socket_response *response = send_and_destroy_request(request);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;

    // handle the response
    switch (gnl_socket_response_type(response)) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error occurred, set the errno
            res = gnl_socket_response_get_error(response);
            GNL_MINUS1_CHECK(res, errno, -1)

            errno = res;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // no need to do something else here
            break;

        default:
            // if this point is reached, the response is not valid
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE|O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

#include <gnl_macro_end.h>