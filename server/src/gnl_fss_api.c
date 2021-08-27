
#include <time.h>
#include <errno.h>
#include <string.h>
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
 * Whether the openFile(pathname, O_CREATE| O_LOCK) api was called.
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
 * {@inheritDoc}
 */
int gnl_fss_api_open_connection(const char *sockname, int msec, const struct timespec abstime) {
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
            errno = EAGAIN;

            return -1;
        }

        // wait the given amount of time
        struct timespec tim;
        tim.tv_sec = 0;
        tim.tv_nsec = 1000 * msec;

        nanosleep(&tim, NULL);
    }

    // initialize the file descriptor table
    file_descriptor_table = NULL;

    // set the connection active
    socket_service_connection_active = 1;

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_close_connection(const char *sockname) {
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

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    // destroy the file descriptor table
    gnl_ternary_search_tree_destroy(&file_descriptor_table, NULL);

    // all the open file will be close by the server as soon
    // as he received the "close connection" socket message

    return res;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_open_file(const char *pathname, int flags) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, pathname, flags);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request to the server
    int bytes_sent = gnl_socket_service_send_request(socket_service_connection, request);
    GNL_MINUS1_CHECK(bytes_sent, errno, -1)

    // clean memory
    gnl_socket_request_destroy(request);

    // get the response from the server
    struct gnl_socket_response *response = gnl_socket_service_get_response(socket_service_connection);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;
    int tmp_res;
    unsigned int *fd_copy = NULL;

    // handle the response
    switch (response->type) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error happen, get the errno
            errno = response->payload.error->number;
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
            // if this point is reached, the response can not be
            // something different from an ok response
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
int gnl_fss_api_read_file(const char *pathname, void **buf, size_t *size) {

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_read_N_files(int N, const char *dirname) {

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_write_file(const char *pathname, const char *dirname) {
    // check if the previous call was to the openFile
    // with the O_CREATE|O_LOCK flags
    if (open_with_create_lock_flags == 0) {
        errno = EPERM;
        return -1;
    }

    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the file to send
    long size; //TODO: size_t
    char *file = NULL;

    int res = gnl_file_to_pointer(pathname, &file, &size);

    GNL_MINUS1_CHECK(res, errno, -1)

    // get the fd associate with the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_WRITE, 2, fd, size, file);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request to the server
    int bytes_sent = gnl_socket_service_send_request(socket_service_connection, request);
    GNL_MINUS1_CHECK(bytes_sent, errno, -1)

    // clean memory
    gnl_socket_request_destroy(request);
    free(file);

    // get the response from the server
    struct gnl_socket_response *response = gnl_socket_service_get_response(socket_service_connection);
    GNL_NULL_CHECK(response, errno, -1)

    res = 0;
    switch (response->type) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error happen, get the errno
            errno = response->payload.error->number;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // success
            break;

        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            // success but one or more files were evicted
            break;

        default:
            // if this point is reached, the response can not be
            // something different from an ok response
            errno = EBADMSG;
            res = -1;
            break;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // if success reset the openFile(pathname, O_CREATE| O_LOCK) check
    if (res == 0) {
        open_with_create_lock_flags = 0;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_append_to_file(const char *pathname, void *buf, size_t size, const char *dirname) {

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_lock_file(const char *pathname) {

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_unlock_file(const char *pathname) {

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_close_file(const char *pathname) {
    // validate the parameters
    GNL_NULL_CHECK(pathname, EINVAL, -1)

    // get the fd associate with the given pathname
    void *fd_raw = gnl_ternary_search_tree_get(file_descriptor_table, pathname);
    GNL_NULL_CHECK(fd_raw, EINVAL, -1);

    int fd = *(int *)fd_raw;

    // free memory
    free(fd_raw);

    // create the request to send to the server
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_CLOSE, 1, fd);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    // send the request to the server
    int bytes_sent = gnl_socket_service_send_request(socket_service_connection, request);
    GNL_MINUS1_CHECK(bytes_sent, errno, -1)

    // clean memory
    gnl_socket_request_destroy(request);

    // get the response from the server
    struct gnl_socket_response *response = gnl_socket_service_get_response(socket_service_connection);
    GNL_NULL_CHECK(response, errno, -1)

    int res = 0;
    int tmp_res;

    // handle the response
    switch (response->type) {

        case GNL_SOCKET_RESPONSE_ERROR:
            // an error happen, get the errno
            errno = response->payload.error->number;
            res = -1;
            break;

        case GNL_SOCKET_RESPONSE_OK:
            // remove the fd from the file_descriptor_table
            tmp_res = gnl_ternary_search_tree_remove(file_descriptor_table, pathname, free);
            GNL_MINUS1_CHECK(tmp_res, errno, -1)

            break;

        default:
            // if this point is reached, the response can not be
            // something different from an ok response
            errno = EBADMSG;
            res = -1;
    }

    // free the memory
    gnl_socket_response_destroy(response);

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return res;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_api_remove_file(const char *pathname) {

    // reset the openFile(pathname, O_CREATE| O_LOCK) check
    open_with_create_lock_flags = 0;

    return 0;
}

#include <gnl_macro_end.h>