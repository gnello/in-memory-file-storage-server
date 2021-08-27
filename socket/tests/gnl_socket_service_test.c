#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_service.c"
#include "../src/gnl_socket_request.c"
#include "../src/gnl_socket_response.c"
#include <gnl_macro_beg.h>

#define SOCKET_NAME "./test.sk"
#define SOCKET_CONNECTION_ATTEMPTS 5
#define SOCKET_CONNECTION_WAIT_SEC 2

#define ALLOCATE_BUFFER(buffer) {                                                                                       \
    GNL_CALLOC(buffer, 256, -1);                                                                                        \
}

#define CONNECT() {                                                                                                     \
    int count = 0;                                                                                                      \
                                                                                                                        \
    while ((connection = gnl_socket_service_connect(SOCKET_NAME)) == NULL) {                                            \
        count++;                                                                                                        \
                                                                                                                        \
        if (count > SOCKET_CONNECTION_ATTEMPTS) {                                                                       \
            perror("gnl_socket_service_connect");                                                                       \
            return -1;                                                                                                  \
        }                                                                                                               \
                                                                                                                        \
        sleep(SOCKET_CONNECTION_WAIT_SEC);                                                                              \
    }                                                                                                                   \
}

#define CHECK_CONNECTION(con, buffer) {                                                                                 \
    gnl_socket_service_readn(con->fd, &buffer, 10);                                                                     \
                                                                                                                        \
    if (strcmp("connected", buffer) != 0) {                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
}

static struct gnl_socket_connection *connection;

int can_not_connect_to_socket() {
    struct gnl_socket_connection *res = gnl_socket_service_connect("nonexistent_socket");

    if (res != NULL) {
        return -1;
    }

    if (errno != ENOENT) {
        return -1;
    }

    return 0;
}

int can_connect_to_socket() {
    int res;

    CONNECT()

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(connection, buffer);

    res = gnl_socket_service_close(connection);
    GNL_MINUS1_CHECK(res, errno, -1)

    free(buffer);

    return 0;
}

int can_connect_twice() {
    int res;
    struct gnl_socket_connection *con2;

    CONNECT()
    char *buffer;

    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(connection, buffer);
    free(buffer);

    con2 = gnl_socket_service_connect(SOCKET_NAME);
    if (con2 == NULL) {
        return -1;
    }

    res = gnl_socket_service_close(connection);
    GNL_MINUS1_CHECK(res, errno, -1)

    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(con2, buffer);
    free(buffer);

    res = gnl_socket_service_close(con2);
    GNL_MINUS1_CHECK(res, errno, -1)

    return 0;
}

int can_not_close_not_open_connection() {
    int res;

    connection = (struct gnl_socket_connection *)calloc(1, sizeof(struct gnl_socket_connection));
    GNL_NULL_CHECK(connection, errno, -1);

    res = gnl_socket_service_close(connection);

    free(connection);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_writen() {
    int res;

    CONNECT()

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(connection, buffer);
    char *message = "Hello World!";

    res = gnl_socket_service_writen(connection->fd, message, strlen(message) + 1);
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_socket_service_readn(connection->fd, &buffer, strlen(message) + 1);

    if (strcmp(message, buffer) != 0) {
        return -1;
    }

    res = gnl_socket_service_close(connection);
    GNL_MINUS1_CHECK(res, errno, -1)

    free(buffer);

    return 0;
}

int can_not_writen() {
    int res;

    connection = (struct gnl_socket_connection *)calloc(1, sizeof(struct gnl_socket_connection));
    GNL_NULL_CHECK(connection, errno, -1);

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    char *message = "Hello World!";

    res = gnl_socket_service_writen(connection->fd, message, strlen(message) + 1);

    free(connection);

    if (res >= 0) {
        return -1;
    }

    free(buffer);

    return 0;
}

int can_readn() {
    int res;

    CONNECT()

    char *buffer;
    ALLOCATE_BUFFER(buffer);
    CHECK_CONNECTION(connection, buffer);

    res = gnl_socket_service_close(connection);
    GNL_MINUS1_CHECK(res, errno, -1)

    free(buffer);

    return 0;
}

int can_not_readn() {
    int res;

    connection = (struct gnl_socket_connection *)calloc(1, sizeof(struct gnl_socket_connection));
    GNL_NULL_CHECK(connection, errno, -1);

    char *buffer;
    ALLOCATE_BUFFER(buffer);

    res = gnl_socket_service_readn(connection->fd, &buffer, 10);

    free(connection);

    free(buffer);

    return !res;
}

int main() {
    gnl_printf_yellow("> gnl_socket_service test:\n\n");

    // connect
    gnl_assert(can_not_connect_to_socket, "can not connect to an nonexistent socket.");
    gnl_assert(can_connect_to_socket, "can connect to an existent socket.");
    gnl_assert(can_connect_twice, "can connect twice to a socket.");

    // close
    gnl_assert(can_not_close_not_open_connection, "can not close a not open connection.");

    // write
    gnl_assert(can_writen, "can write N bytes in a socket.");
    gnl_assert(can_not_writen, "can not write in a not open connection.");

    // read
    gnl_assert(can_readn, "can read N bytes from a socket.");
    gnl_assert(can_not_readn, "can not read N bytes from a not open connection.");

    printf("\n");
}

#undef SOCKET_NAME
#undef SOCKET_CONNECTION_ATTEMPTS
#undef SOCKET_CONNECTION_WAIT_SEC
#undef ALLOCATE_BUFFER
#undef CONNECT
#undef CHECK_CONNECTION

#include <gnl_macro_end.h>