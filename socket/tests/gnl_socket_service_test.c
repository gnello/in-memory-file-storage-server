#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_service.c"
#include <gnl_macro_beg.h>

#define SOCKET_NAME "./test.sk"

static int socket_create() {
    int fd_skt;
    int res;

    // create socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, SOCKET_NAME, strlen(SOCKET_NAME) + 1);
    sa.sun_family = AF_UNIX;

    // create the server
    fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    GNL_MINUS1_CHECK(fd_skt, errno, -1);

    // bind the address
    res = bind(fd_skt, (struct sockaddr *)&sa, sizeof(sa));
    GNL_MINUS1_CHECK(res, errno, -1);

    // listen
    res = listen(fd_skt, SOMAXCONN);
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

static int socket_destroy() {
    remove(SOCKET_NAME);

    return 0;
}

int can_not_connect_to_socket() {
    gnl_socket_service_connect("nonexistent_socket");

    if (errno != ENOENT) {
        return -1;
    }

    return 0;
}

int can_connect_to_socket() {
    int res;

    socket_create();

    res = gnl_socket_service_connect(SOCKET_NAME);
    gnl_socket_service_close(SOCKET_NAME);

    socket_destroy();

    return res;
}

int can_not_connect_twice() {
    int res;

    socket_create();

    res = gnl_socket_service_connect(SOCKET_NAME);
    if (res == 0) {
        res = gnl_socket_service_connect(SOCKET_NAME);
    }

    gnl_socket_service_close(SOCKET_NAME);
    socket_destroy();

    return !res;
}

int can_not_close_any_connection() {
    int res;

    socket_create();

    res = gnl_socket_service_connect(SOCKET_NAME);
    if (res == 0) {
        res = gnl_socket_service_close("./any_socket_name.sk");
    }

    gnl_socket_service_close(SOCKET_NAME);
    socket_destroy();

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_not_close_not_open_connection() {
    int res;

    res = gnl_socket_service_close(SOCKET_NAME);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_service test:\n\n");

    gnl_assert(can_not_connect_to_socket, "can not connect to an nonexistent socket.");
    gnl_assert(can_connect_to_socket, "can connect to an existent socket.");

    gnl_assert(can_not_connect_twice, "can not connect twice to any socket.");
    gnl_assert(can_not_close_any_connection, "can not close a connection with different socket name.");
    gnl_assert(can_not_close_not_open_connection, "can not close a not open connection.");



    printf("\n");
}

#include <gnl_macro_end.h>