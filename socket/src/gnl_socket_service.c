#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include "../include/gnl_socket_service.h"
#include "./gnl_socket_rts.c"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
int gnl_socket_service_is_active(const struct gnl_socket_connection *connection) {
    if (connection == NULL) {
        return 0;
    }

    return connection->active;
}

/**
 * {@inheritDoc}
 */
struct gnl_socket_connection *gnl_socket_service_connect(const char *socket_name) {
    struct gnl_socket_connection *connection = (struct gnl_socket_connection *)malloc(sizeof(struct gnl_socket_connection));
    GNL_NULL_CHECK(connection, ENOMEM, NULL)

    // copy the socket name
    connection->socket_name = (char *)calloc(strlen(socket_name) + 1, sizeof(char));
    if (connection->socket_name == NULL) {
        free(connection);

        return NULL;
    }

    strncpy(connection->socket_name, socket_name, strlen(socket_name));

    // create the socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, socket_name, strlen(socket_name) + 1);
    sa.sun_family = AF_UNIX;

    // create the socket and get his file descriptor
    connection->fd = socket(AF_UNIX,SOCK_STREAM,0);
    if (connection->fd == -1) {
        free(connection->socket_name);
        free(connection);

        return NULL;
    }

    // connect to the socket
    int res = connect(connection->fd, (struct sockaddr *)&sa, sizeof(sa));
    if (res == -1) {
        free(connection->socket_name);
        free(connection);

        // let the errno bubble

        return NULL;
    }

    // activate flag
    connection->active = 1;

    return connection;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_service_close(struct gnl_socket_connection *connection) {
    if (!gnl_socket_service_is_active(connection)) {
        errno = EINVAL;

        return -1;
    }

    int res = close(connection->fd);
    GNL_MINUS1_CHECK(res, errno, -1);

    // destroy connection
    free(connection->socket_name);
    free(connection);

    return 0;
}

/**
 * {@inheritDoc}
 */
ssize_t gnl_socket_service_writen(int fd, void *ptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;

    nleft = n;

    while (nleft > 0) {

        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n) {
                // error, return -1
                return -1;
            } else {
                // error, return amount written so far
                break;
            }
        } else if (nwritten == 0) {
            break;
        }

        nleft -= nwritten;
        (char *)ptr += nwritten;
    }

    // return >= 0
    return (n - nleft);
}

/**
 * {@inheritDoc}
 */
ssize_t gnl_socket_service_readn(int fd, void *ptr, size_t n) {
    size_t nleft;
    ssize_t nread;

    nleft = n;

    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n) {
                // error, return -1
                return -1;
            } else {
                // error, return amount read so far
                break;
            }
        } else if (nread == 0) {
            // EOF
            break;
        }

        nleft -= nread;
        (char *)ptr += nread;
    }

    // return >= 0
    return(n - nleft);
}

/**
 * {@inheritDoc}
 */
struct gnl_socket_response * gnl_socket_service_send(const struct gnl_socket_connection *connection,
                                                     const struct gnl_socket_request *request) {
    //validate parameters
    GNL_NULL_CHECK(connection, EINVAL, NULL);
    GNL_NULL_CHECK(request, EINVAL, NULL);

    // check if the connection is active
    if (!gnl_socket_service_is_active(connection)) {
        errno = EINVAL;

        return NULL;
    }

    // send the request through the socket
    size_t bytes = gnl_socket_request_write(connection->fd, request, gnl_socket_service_writen);
    GNL_MINUS1_CHECK(bytes, errno, NULL)

    // wait for the response

    return res;
}

#include <gnl_macro_end.h>