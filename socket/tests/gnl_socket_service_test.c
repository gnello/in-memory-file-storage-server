#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_service.c"
#include <gnl_macro_beg.h>

#define SOCKET_NAME "./test.sk"
#define SOCKET_CONNECTION_ATTEMPTS 5
#define SOCKET_CONNECTION_WAIT_SEC 2

#define ALLOCATE_BUFFER(buffer) {       \
    GNL_ALLOCATE_MESSAGE(buffer, 256);  \
    GNL_NULL_CHECK(buffer, ENOMEM, -1)  \
}

#define CHECK_CONNECTION(buffer) {          \
    gnl_socket_service_read(&buffer, 256);  \
                                            \
    if (strcmp("connected", buffer) != 0) { \
        return -1;                          \
    }                                       \
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

    res = gnl_socket_service_connect(SOCKET_NAME);
    GNL_MINUS1_CHECK(res, errno, -1)

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(buffer);

    res = gnl_socket_service_close(SOCKET_NAME);
    GNL_MINUS1_CHECK(res, errno, -1)

    return 0;
}

int can_not_connect_twice() {
    int res;

    res = gnl_socket_service_connect(SOCKET_NAME);
    GNL_MINUS1_CHECK(res, errno, -1)

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(buffer);

    res = gnl_socket_service_connect(SOCKET_NAME);
    if (res != -1) {
        return -1;
    }

    res = gnl_socket_service_close(SOCKET_NAME);
    GNL_MINUS1_CHECK(res, errno, -1)

    return 0;
}

int can_not_close_any_connection() {
    int res;

    res = gnl_socket_service_connect(SOCKET_NAME);
    GNL_MINUS1_CHECK(res, errno, -1)

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(buffer);

    res = gnl_socket_service_close("./any_socket_name.sk");

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    res = gnl_socket_service_close(SOCKET_NAME);
    GNL_MINUS1_CHECK(res, errno, -1)

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

int can_emit() {
    int res;
    int count = 0;

    while (gnl_socket_service_connect(SOCKET_NAME) == -1) {
        count++;

        if (count > SOCKET_CONNECTION_ATTEMPTS) {
            perror("gnl_socket_service_connect");
            return -1;
        }

        sleep(SOCKET_CONNECTION_WAIT_SEC);
    }

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(buffer);
    char *message = "Hello World!";

    res = gnl_socket_service_emit(message);
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_socket_service_read(&buffer, 256);

    if (strcmp(message, buffer) != 0) {
        return -1;
    }

    gnl_socket_service_close(SOCKET_NAME);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_service test:\n\n");

    // connect
    gnl_assert(can_not_connect_to_socket, "can not connect to an nonexistent socket.");
    gnl_assert(can_connect_to_socket, "can connect to an existent socket.");
    gnl_assert(can_not_connect_twice, "can not connect twice to any socket.");

    // close
    gnl_assert(can_not_close_any_connection, "can not close a connection with different socket name.");
    gnl_assert(can_not_close_not_open_connection, "can not close a not open connection.");

    // emit
    gnl_assert(can_emit, "can send messages throw the socket.");

    printf("\n");
}

#undef SOCKET_NAME
#undef SOCKET_CONNECTION_ATTEMPTS
#undef SOCKET_CONNECTION_WAIT_SEC
#undef ALLOCATE_BUFFER
#undef CHECK_CONNECTION

#include <gnl_macro_end.h>