#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include "../include/gnl_socket_service.h"
#include "./gnl_socket_rts.c"
#include <gnl_macro_beg.h>

int gnl_socket_service_is_active(const struct gnl_socket_connection *connection) {
    if (connection == NULL) {
        return 0;
    }

    return connection->active;
}

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
    if (res != 0) {
        free(connection->socket_name);
        free(connection);

        return NULL;
    }

    // activate flag
    connection->active = 1;

    return connection;
}

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

int gnl_socket_service_emit(const struct gnl_socket_connection *connection, const char *message) {
    if (!gnl_socket_service_is_active(connection)) {
        errno = EINVAL;

        return -1;
    }

    return write(connection->fd, (char *)message, strlen(message) + 1);
}

int gnl_socket_service_read(const struct gnl_socket_connection *connection, char **message, int size) {
    if (!gnl_socket_service_is_active(connection)) {
        errno = EINVAL;

        return -1;
    }

    return read(connection->fd, *message, size);
}

#include <gnl_macro_end.h>