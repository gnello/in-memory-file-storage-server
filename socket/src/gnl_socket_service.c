#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include "../include/gnl_socket_service.h"
#include <gnl_macro_beg.h>

/**
 * Socket connection support.
 */
struct socket_service_connection {
    int fd; // the identifier of the socket connection
    char* socket_name; // the socket name
    int flag; // if 0 the struct was not initialized, if 1 was it
};

static struct socket_service_connection socket_service_connection;

int gnl_socket_service_connect(const char *socket_name) {
    if (socket_service_connection.flag == 1) {
        errno = EINVAL;

        return -1;
    }

    // copy the socket name
    GNL_CALLOC(socket_service_connection.socket_name, strlen(socket_name) + 1, -1)

    strncpy(socket_service_connection.socket_name, socket_name, strlen(socket_name));

    // create the socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, socket_name, strlen(socket_name) + 1);
    sa.sun_family = AF_UNIX;

    // create the socket and get his file descriptor
    socket_service_connection.fd = socket(AF_UNIX,SOCK_STREAM,0);
    GNL_MINUS1_CHECK(socket_service_connection.fd, errno, -1);

    // connect to the socket
    int res = connect(socket_service_connection.fd, (struct sockaddr *)&sa, sizeof(sa));
    GNL_MINUS1_CHECK(res, errno, -1);

    // activate flag
    socket_service_connection.flag = 1;

    return 0;
}

int gnl_socket_service_close(const char *socket_name) {
    if (socket_service_connection.flag == 0 || strcmp(socket_service_connection.socket_name, socket_name) != 0) {
        errno = EINVAL;

        return -1;
    }

    int res;

    res = close(socket_service_connection.fd);
    GNL_MINUS1_CHECK(res, errno, -1);

    // deactivate flag
    socket_service_connection.flag = 0;

    // reset struct
    socket_service_connection.fd = 0;
    free(socket_service_connection.socket_name);

    return 0;
}

int gnl_socket_service_emit(const char *message) {
    if (socket_service_connection.flag == 0) {
        errno = EINVAL;

        return -1;
    }

    return write(socket_service_connection.fd, (char *)message, strlen(message) + 1);
}

int gnl_socket_service_read(char **message, int size) {
    if (socket_service_connection.flag == 0) {
        errno = EINVAL;

        return -1;
    }

    return read(socket_service_connection.fd, *message, size);
}

#include <gnl_macro_end.h>