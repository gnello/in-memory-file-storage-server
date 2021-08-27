#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include "../include/gnl_socket_service.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10
#define PROTOCOL_SIZE (MAX_DIGITS_INT + MAX_DIGITS_INT)

/**
 * Write a socket message into the given file descriptor.
 *
 * @param fd        The file descriptor where to write the message.
 * @param message   The message to write.
 * @param type      The operation type to encode.
 * @param count     The number of bytes of the message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static size_t write_protocol_message(int fd, const char *message, int type, size_t count) {
    // validate parameters
    // if count is > 0 then the message must not be null
    if (count > 0) {
        GNL_NULL_CHECK(message, EINVAL, -1);
    }

    char *protocol_message;

    // allocate memory for the protocol message building
    GNL_CALLOC(protocol_message, PROTOCOL_SIZE + 1 + count, -1)

    int maxlen = PROTOCOL_SIZE + 1; // count also the '\0' char

    // add the protocol metadata
    snprintf(protocol_message, maxlen, "%0*d%0*lu", MAX_DIGITS_INT, type, MAX_DIGITS_INT, count);

    // add the rest of the message (if the count is > 0)
    if (count > 0) {
        memcpy(protocol_message + maxlen, message, count);
    }

    int len = maxlen + count;

    // send the request
    int nwrite = gnl_socket_service_writen(fd, protocol_message, len);

    // free memory
    free(protocol_message);

    // check the result of the sending
    if (nwrite <= 0) {
        // let the errno bubble

        return -1;
    }

    return nwrite;
}

/**
 * Read a socket message from a file descriptor. If an error occurs,
 * the given "dest" pointer value is unpredictable and it should not
 * be used. No leaks are raised.
 *
 * @param fd        The file descriptor where to read.
 * @param dest      The destination where to put the socket message.
 * @param type      The pointer where to put the operation type.
 *
 * @return          Returns the number of bytes read on success,
 *                  -1 otherwise.
 */
static size_t read_protocol_message(int fd, char **dest, int *type) {
    char *protocol_message;
    size_t message_len;

    // allocate memory for the initial message reading
    GNL_CALLOC(protocol_message, PROTOCOL_SIZE + 1, -1)

    // read the first 21 chars, the protocol standard puts
    // the type of the message in the first 10 chars and the
    // size of the message in the second 10 chars, the remaining
    // byte is the null terminator char
    ssize_t proto_nread = gnl_socket_service_readn(fd, protocol_message, PROTOCOL_SIZE + 1);

    // if nread == 0, the connection is closed, return
    if (proto_nread == 0) {
        free(protocol_message);

        return 0;
    }

    // check if the read succeeded
    if (proto_nread != (PROTOCOL_SIZE + 1)) {
        free(protocol_message);

        // if nread == -1, let the errno bubble
        if (proto_nread != -1) {
            errno = EBADMSG;
        }

        return -1;
    }

    // get the operation type and the message length
    sscanf(protocol_message, "%"MAX_DIGITS_CHAR"d%"MAX_DIGITS_CHAR"lu", type, &message_len);

    // free memory
    free(protocol_message);

    // check the message_len
    if (message_len <= 0) {
        errno = EBADMSG;

        return -1;
    }

    // allocate memory for the payload message
    GNL_CALLOC(*dest, message_len, -1)

    // get the message
    size_t nread = gnl_socket_service_readn(fd, *dest, message_len);

    // check if the read succeeded
    if (nread != message_len) {
        free(*dest);
        *dest = NULL;

        // if nread == -1, let the errno bubble
        if (nread != -1) {
            errno = EBADMSG;
        }

        return -1;
    }

    return proto_nread + nread;
}

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
        ptr = (char *)ptr + nwritten;
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
        ptr = (char *)ptr + nread;
    }

    // return >= 0
    return(n - nleft);
}

/**
 * {@inheritDoc}
 */
int gnl_socket_service_send_request(const struct gnl_socket_connection *connection,
        const struct gnl_socket_request *request) {

    //validate parameters
    GNL_NULL_CHECK(connection, EINVAL, -1);
    GNL_NULL_CHECK(request, EINVAL, -1);

    // check if the connection is active
    if (!gnl_socket_service_is_active(connection)) {
        errno = EINVAL;

        return -1;
    }

    char *message = NULL;

    // get the request message
    size_t bytes = gnl_socket_request_to_string(request, &message);

    if (bytes == -1) {
        free(message);
        // let the errno bubble

        return -1;
    }

    // send the request message through the socket
    size_t nwrite = write_protocol_message(connection->fd, message, request->type, bytes);

    // free memory
    free(message);

    // check the writing result
    GNL_MINUS1_CHECK(nwrite, errno, -1)

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_socket_request *gnl_socket_service_get_request(int fd) {
    char *message = NULL;
    int type;

    size_t nread = read_protocol_message(fd, &message, &type);

    // check the reading result
    if (nread == -1) {
        free(message);
        // let the errno bubble

        return NULL;
    }

    // if nread == 0 the connection is closed, return
    if (nread == 0) {
        errno = EPIPE;

        return NULL;
    }

    // get the request from the message
    struct gnl_socket_request *request;
    request = gnl_socket_request_from_string(message, type);

    // free memory
    free(message);

    // check the request
    GNL_NULL_CHECK(request, errno, NULL);

    return request;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_service_send_response(int fd, const struct gnl_socket_response *response) {
    //validate parameters
    GNL_NULL_CHECK(response, EINVAL, -1);

    char *message = NULL;

    // get the request message
    size_t bytes = gnl_socket_response_to_string(response, &message);

    if (bytes == -1) {
        free(message);
        // let the errno bubble

        return -1;
    }

    // send the request message through the socket
    size_t nwrite = write_protocol_message(fd, message, response->type, bytes);

    // free memory
    free(message);

    // check the writing result
    GNL_MINUS1_CHECK(nwrite, errno, -1)

    return 0;
}

/**
 * {@inheritDoc}
 */ //TODO: se è richiesto di loggare i byte lato client allora ritornare i byte
struct gnl_socket_response *gnl_socket_service_get_response(const struct gnl_socket_connection *connection) {
    //validate parameters
    GNL_NULL_CHECK(connection, EINVAL, NULL);

    // check if the connection is active
    if (!gnl_socket_service_is_active(connection)) {
        errno = EINVAL;

        return NULL;
    }

    char *message = NULL;
    int type;

    size_t nread = read_protocol_message(connection->fd, &message, &type);

    // check the reading result
    if (nread == -1) {
        free(message);
        // let the errno bubble

        return NULL;
    }

    // if nread == 0 the connection is closed, return
    if (nread == 0) {
        errno = EPIPE;

        return NULL;
    }

    // get the request from the message
    struct gnl_socket_response *response;
    response = gnl_socket_response_from_string(message, type);

    // free memory
    free(message);

    // check the request
    GNL_NULL_CHECK(response, errno, NULL);

    return response;
}

#undef MAX_DIGITS_CHAR
#undef MAX_DIGITS_INT
#undef PROTOCOL_SIZE
#include <gnl_macro_end.h>