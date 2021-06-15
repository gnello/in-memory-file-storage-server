#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_request.c"

#define GNL_TEST_EMPTY_GENERIC(request_type) {                                              \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 0);  \
                                                                                            \
    if (request == NULL) {                                                                  \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    if (request->type != request_type) {                                                    \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    if (request->payload.read->string != NULL) {                                          \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    gnl_socket_request_destroy(request);                                                \
                                                                                            \
    return 0;                                                                               \
}

#define GNL_TEST_GENERIC_ARGS(request_type, ref) {                                                          \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, "/fake/path");    \
                                                                                                            \
    if (request == NULL) {                                                                                  \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    if (request->type != request_type) {                                                                    \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    if (strcmp(ref->string, "/fake/path") != 0) {                                                         \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    gnl_socket_request_destroy(request);                                                                \
                                                                                                            \
    return 0;                                                                                               \
}

#define GNL_TEST_GENERIC_READ(request_type, ref) {                              \
    struct gnl_socket_request *request;                                     \
                                                                                \
    char message[41];                                                           \
    sprintf(message, "%0*d00000000200000000010/fake/path", 10, request_type);   \
                                                                                \
    request = gnl_socket_request_read(message);                             \
    if (request == NULL) {                                                      \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (request->type != request_type) {                                        \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (strcmp(ref->string, "/fake/path") != 0) {                             \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    gnl_socket_request_destroy(request);                                    \
                                                                                \
    return 0;                                                                   \
}

#define GNL_TEST_GENERIC_WRITE(request_type) {                                                              \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, "/fake/path");    \
                                                                                                            \
    if (request == NULL) {                                                                                  \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    char *message;                                                                                          \
    gnl_socket_request_write(request, &message);                                                        \
                                                                                                            \
    char expected[41];                                                                                      \
    sprintf(expected, "%0*d00000000200000000010/fake/path", 10, request_type);                              \
                                                                                                            \
    if (strcmp(expected, message) != 0) {                                                                   \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    gnl_socket_request_destroy(request);                                                                \
    free(message);                                                                                          \
                                                                                                            \
    return 0;                                                                                               \
}

int can_init_empty_open() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 0);

    if (request == NULL) {
        return -1;
    }

    if (request->type != GNL_SOCKET_REQUEST_OPEN) {
        return -1;
    }


    if (request->payload.open->string != NULL) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_init_args_open() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, "/fake/path", 2);

    if (request == NULL) {
        return -1;
    }

    if (request->type != GNL_SOCKET_REQUEST_OPEN) {
        return -1;
    }

    if (strcmp(request->payload.open->string, "/fake/path") != 0) {
        return -1;
    }

    if (request->payload.open->number != 2) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_read_open() {
    struct gnl_socket_request *request;

    request = gnl_socket_request_read("000000000000000000300000000010/fake/path0000000003");
    if (request == NULL) {
        return -1;
    }

    if (request->type != GNL_SOCKET_REQUEST_OPEN) {
        return -1;
    }

    if (strcmp(request->payload.open->string, "/fake/path") != 0) {
        return -1;
    }

    if (request->payload.open->number != 3) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_write_open() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, "/fake/path", 3);

    if (request == NULL) {
        return -1;
    }

    char *message;
    gnl_socket_request_write(request, &message);

    if (strcmp("000000000000000000300000000010/fake/path0000000003", message) != 0) {
        return -1;
    }

    gnl_socket_request_destroy(request);
    free(message);

    return 0;
}

int can_init_empty_read_N() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_READ_N, 0);

    if (request == NULL) {
        return -1;
    }

    if (request->type != GNL_SOCKET_REQUEST_READ_N) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_init_args_read_N() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_READ_N, 1, 15);

    if (request == NULL) {
        return -1;
    }

    if (request->type != GNL_SOCKET_REQUEST_READ_N) {
        return -1;
    }

    if (request->payload.read_N->number != 15) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_read_read_N() {
    struct gnl_socket_request *request;

    request = gnl_socket_request_read("000000000200000000100000000015");
    if (request == NULL) {
        return -1;
    }

    if (request->type != GNL_SOCKET_REQUEST_READ_N) {
        return -1;
    }

    if (request->payload.read_N->number != 15) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_write_read_N() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_READ_N, 1, 15);

    if (request == NULL) {
        return -1;
    }

    char *message;
    gnl_socket_request_write(request, &message);

    if (strcmp("000000000200000000100000000015", message) != 0) {
        return -1;
    }

    gnl_socket_request_destroy(request);
    free(message);

    return 0;
}

int can_init_empty_read() {
    GNL_TEST_EMPTY_GENERIC(GNL_SOCKET_REQUEST_READ)
}

int can_init_args_read() {
    GNL_TEST_GENERIC_ARGS(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_read_read() {
    GNL_TEST_GENERIC_READ(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_write_read() {
    GNL_TEST_GENERIC_WRITE(GNL_SOCKET_REQUEST_READ)
}

int can_init_empty_write() {
    GNL_TEST_EMPTY_GENERIC(GNL_SOCKET_REQUEST_WRITE)
}

int can_init_args_write() {
    GNL_TEST_GENERIC_ARGS(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_read_write() {
    GNL_TEST_GENERIC_READ(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_write_write() {
    GNL_TEST_GENERIC_WRITE(GNL_SOCKET_REQUEST_WRITE)
}

int can_init_empty_lock() {
    GNL_TEST_EMPTY_GENERIC(GNL_SOCKET_REQUEST_LOCK)
}

int can_init_args_lock() {
    GNL_TEST_GENERIC_ARGS(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_read_lock() {
    GNL_TEST_GENERIC_READ(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_write_lock() {
    GNL_TEST_GENERIC_WRITE(GNL_SOCKET_REQUEST_LOCK)
}

int can_init_empty_unlock() {
    GNL_TEST_EMPTY_GENERIC(GNL_SOCKET_REQUEST_UNLOCK)
}

int can_init_args_unlock() {
    GNL_TEST_GENERIC_ARGS(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_read_unlock() {
    GNL_TEST_GENERIC_READ(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_write_unlock() {
    GNL_TEST_GENERIC_WRITE(GNL_SOCKET_REQUEST_UNLOCK)
}

int can_init_empty_close() {
    GNL_TEST_EMPTY_GENERIC(GNL_SOCKET_REQUEST_CLOSE)
}

int can_init_args_close() {
    GNL_TEST_GENERIC_ARGS(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_read_close() {
    GNL_TEST_GENERIC_READ(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_write_close() {
    GNL_TEST_GENERIC_WRITE(GNL_SOCKET_REQUEST_CLOSE)
}

int can_init_empty_remove() {
    GNL_TEST_EMPTY_GENERIC(GNL_SOCKET_REQUEST_REMOVE)
}

int can_init_args_remove() {
    GNL_TEST_GENERIC_ARGS(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_read_remove() {
    GNL_TEST_GENERIC_READ(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_write_remove() {
    GNL_TEST_GENERIC_WRITE(GNL_SOCKET_REQUEST_REMOVE)
}

int main() {
    gnl_printf_yellow("> gnl_socket_request test:\n\n");

    gnl_assert(can_init_empty_open, "can init an empty GNL_SOCKET_REQUEST_OPEN request type.");
    gnl_assert(can_init_args_open, "can init a GNL_SOCKET_REQUEST_OPEN request type with args.");
    gnl_assert(can_read_open, "can read a GNL_SOCKET_REQUEST_OPEN request type message.");
    gnl_assert(can_write_open, "can write a GNL_SOCKET_REQUEST_OPEN request type.");

    gnl_assert(can_init_empty_read, "can init an empty GNL_SOCKET_REQUEST_READ request type.");
    gnl_assert(can_init_args_read, "can init a GNL_SOCKET_REQUEST_READ request type with args.");
    gnl_assert(can_read_read, "can read a GNL_SOCKET_REQUEST_READ request type message.");
    gnl_assert(can_write_read, "can write a GNL_SOCKET_REQUEST_READ request type.");

    gnl_assert(can_init_empty_read_N, "can init an empty GNL_SOCKET_REQUEST_READ_N request type.");
    gnl_assert(can_init_args_read_N, "can init a GNL_SOCKET_REQUEST_READ_N request type with args.");
    gnl_assert(can_read_read_N, "can read a GNL_SOCKET_REQUEST_READ_N request type message.");
    gnl_assert(can_write_read_N, "can write a GNL_SOCKET_REQUEST_READ_N request type.");

    gnl_assert(can_init_empty_write, "can init an empty GNL_SOCKET_REQUEST_WRITE request type.");
    gnl_assert(can_init_args_write, "can init a GNL_SOCKET_REQUEST_WRITE request type with args.");
    gnl_assert(can_read_write, "can read a GNL_SOCKET_REQUEST_WRITE request type message.");
    gnl_assert(can_write_write, "can write a GNL_SOCKET_REQUEST_WRITE request type.");

    gnl_assert(can_init_empty_lock, "can init an empty GNL_SOCKET_REQUEST_LOCK request type.");
    gnl_assert(can_init_args_lock, "can init a GNL_SOCKET_REQUEST_LOCK request type with args.");
    gnl_assert(can_read_lock, "can read a GNL_SOCKET_REQUEST_LOCK request type message.");
    gnl_assert(can_write_lock, "can write a GNL_SOCKET_REQUEST_LOCK request type.");

    gnl_assert(can_init_empty_unlock, "can init an empty GNL_SOCKET_REQUEST_UNLOCK request type.");
    gnl_assert(can_init_args_unlock, "can init a GNL_SOCKET_REQUEST_UNLOCK request type with args.");
    gnl_assert(can_read_unlock, "can read a GNL_SOCKET_REQUEST_UNLOCK request type message.");
    gnl_assert(can_write_unlock, "can write a GNL_SOCKET_REQUEST_UNLOCK request type.");

    gnl_assert(can_init_empty_close, "can init an empty GNL_SOCKET_REQUEST_CLOSE request type.");
    gnl_assert(can_init_args_close, "can init a GNL_SOCKET_REQUEST_CLOSE request type with args.");
    gnl_assert(can_read_close, "can read a GNL_SOCKET_REQUEST_CLOSE request type message.");
    gnl_assert(can_write_close, "can write a GNL_SOCKET_REQUEST_CLOSE request type.");

    gnl_assert(can_init_empty_remove, "can init an empty GNL_SOCKET_REQUEST_REMOVE request type.");
    gnl_assert(can_init_args_remove, "can init a GNL_SOCKET_REQUEST_REMOVE request type with args.");
    gnl_assert(can_read_remove, "can read a GNL_SOCKET_REQUEST_REMOVE request type message.");
    gnl_assert(can_write_remove, "can write a GNL_SOCKET_REQUEST_REMOVE request type.");

    // the gnl_socket_request_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}

#undef GNL_TEST_EMPTY_GENERIC
#undef GNL_TEST_GENERIC_ARGS
#undef GNL_TEST_GENERIC_READ
#undef GNL_TEST_GENERIC_WRITE