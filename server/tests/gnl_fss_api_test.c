#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_fss_api.c"
#include "./mocks/gnl_socket_service.c"
#include <gnl_macro_beg.h>

#define SOCKET_NAME "./test.sk"

int can_not_connect() {
    mock_gnl_socket_service_set_connect_result(-1);

    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    return !openConnection(SOCKET_NAME, 100, tim);
}

int can_connect() {
    int res;

    mock_gnl_socket_service_set_connect_result(0);
    mock_gnl_socket_service_set_close_connection_result(0);

    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    res = openConnection(SOCKET_NAME, 100, tim);

    closeConnection(SOCKET_NAME);

    return res;
}

int can_not_accept_null_socket() {
    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    char *null_socket_name = NULL;
    int res = openConnection(null_socket_name, 100, tim);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_not_accept_0_msec() {
    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    int res = openConnection(SOCKET_NAME, 0, tim);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_not_accept_negative_msec() {
    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    int res = openConnection(SOCKET_NAME, -10, tim);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_not_connect_twice() {
    int res;

    mock_gnl_socket_service_set_connect_result(0);
    mock_gnl_socket_service_set_close_connection_result(0);

    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    res = openConnection(SOCKET_NAME, 100, tim);
    GNL_MINUS1_CHECK(res, errno, -1)

    res = openConnection(SOCKET_NAME, 100, tim);
    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    closeConnection(SOCKET_NAME);

    return 0;
}

int can_not_close() {
    mock_gnl_socket_service_set_close_connection_result(-1);

    return !closeConnection(SOCKET_NAME);
}

int can_not_close_different() {
    mock_gnl_socket_service_set_connect_result(0);

    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    int res = openConnection(SOCKET_NAME, 100, tim);
    GNL_MINUS1_CHECK(res, errno, -1)

    mock_gnl_socket_service_set_close_connection_result(0);

    res = closeConnection("./tmp2.ssk");
    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return closeConnection(SOCKET_NAME);
}

int can_close() {
    mock_gnl_socket_service_set_connect_result(0);

    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;

    int res = openConnection(SOCKET_NAME, 100, tim);
    GNL_MINUS1_CHECK(res, errno, -1)

    mock_gnl_socket_service_set_close_connection_result(0);

    return closeConnection(SOCKET_NAME);
}

int main() {
    gnl_printf_yellow("> gnl_fss_api test:\n\n");

    // openConnection
    gnl_assert(can_not_connect, "can not connect to a socket.");
    gnl_assert(can_connect, "can connect to a socket.");
    gnl_assert(can_not_accept_null_socket, "can not accept a null socket name to open a socket connection.");
    gnl_assert(can_not_accept_0_msec, "can not accept a msec value equal to zero to open a socket connection.");
    gnl_assert(can_not_accept_negative_msec, "can not accept a msec value less than zero to open a socket connection.");
    gnl_assert(can_not_connect_twice, "can not connect to a socket more than once per time.");

    // closeConnection
    gnl_assert(can_not_close, "can not close a connection to a socket.");
    gnl_assert(can_not_close_different, "can not close a connection to a different socket.");
    gnl_assert(can_close, "can close a connection to a socket.");

    // openFile

    printf("\n");
}

#include <gnl_macro_end.h>