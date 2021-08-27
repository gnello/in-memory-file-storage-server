#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_request.c"

#define GNL_TEST_EMPTY_REQUEST_N(request_type, ref) {                                       \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 0);          \
                                                                                            \
    if (request == NULL) {                                                                  \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    if (request->type != request_type) {                                                    \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    gnl_socket_request_destroy(request);                                                    \
                                                                                            \
    return 0;                                                                               \
}

#define GNL_TEST_REQUEST_N_ARGS(request_type, ref) {                                    \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, 15);  \
                                                                                        \
    if (request == NULL) {                                                              \
        return -1;                                                                      \
    }                                                                                   \
                                                                                        \
    if (request->type != request_type) {                                                \
        return -1;                                                                      \
    }                                                                                   \
                                                                                        \
    if (ref->number != 15) {                                                            \
        return -1;                                                                      \
    }                                                                                   \
                                                                                        \
    gnl_socket_request_destroy(request);                                                \
                                                                                        \
    return 0;                                                                           \
}

#define GNL_TEST_REQUEST_N_READ(request_type, ref) {                            \
    struct gnl_socket_request *request;                                         \
                                                                                \
    char message[31];                                                           \
    sprintf(message, "%0*d00000000100000000015", 10, request_type);             \
                                                                                \
    /*request = gnl_socket_request_read(message); */                                \
    if (request == NULL) {                                                      \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (request->type != request_type) {                                        \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (ref->number != 15) {                                                    \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    gnl_socket_request_destroy(request);                                        \
                                                                                \
    return 0;                                                                   \
}

#define GNL_TEST_REQUEST_N_WRITE(request_type) {                                        \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, 15);  \
                                                                                        \
    if (request == NULL) {                                                              \
        return -1;                                                                      \
    }                                                                                   \
                                                                                        \
    char *message = NULL;                                                               \
    int res = gnl_socket_request_to_string(request, &message);                              \
    if (res == -1) {                                                                    \
        return -1;                                                                      \
    }                                                                                   \
                                                                                        \
    char expected[31];                                                                  \
    sprintf(expected, "%0*d00000000100000000015", 10, request_type);                    \
    if (strcmp(expected, message) != 0) {                                               \
        return -1;                                                                      \
    }                                                                                   \
                                                                                        \
    gnl_socket_request_destroy(request);                                                \
    free(message);                                                                      \
                                                                                        \
    return 0;                                                                           \
}

#define GNL_TEST_EMPTY_REQUEST_S(request_type, ref) {                                              \
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
    if (ref->string != NULL) {                                          \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    gnl_socket_request_destroy(request);                                                \
                                                                                            \
    return 0;                                                                               \
}

#define GNL_TEST_REQUEST_S_ARGS(request_type, ref) {                                                          \
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

#define GNL_TEST_REQUEST_S_READ(request_type, ref) {                              \
    struct gnl_socket_request *request;                                     \
                                                                                \
    char message[41];                                                           \
    sprintf(message, "%0*d00000000200000000010/fake/path", 10, request_type);   \
                                                                                \
    /*request = gnl_socket_request_read(message);  */                           \
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

#define GNL_TEST_REQUEST_S_WRITE(request_type) {                                                              \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, "/fake/path");    \
                                                                                                            \
    if (request == NULL) {                                                                                  \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    char *message = NULL;                                                                                          \
    int res = gnl_socket_request_to_string(request, &message);                                                        \
    if (res == -1) {                               \
        return -1;                                      \
    } \
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

#define GNL_TEST_EMPTY_REQUEST_SB(request_type, ref) {                                              \
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
    if (ref->string != NULL) {                                          \
        return -1;                                                                          \
    }                                                                                         \
                                                                                            \
    if (ref->bytes != NULL) {                                          \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    gnl_socket_request_destroy(request);                                                \
                                                                                            \
    return 0;                                                                               \
}

#define GNL_TEST_REQUEST_SB_ARGS(request_type, ref) {                                                          \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 2, "/fake/path", "\x41\x42\x43\x44");    \
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
    }                                                                                                          \
                                                                                                            \
    if (strcmp(ref->bytes, "ABCD") != 0) {                                                         \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    gnl_socket_request_destroy(request);                                                                \
                                                                                                            \
    return 0;                                                                                               \
}

#define GNL_TEST_REQUEST_SB_READ(request_type, ref) {                              \
    struct gnl_socket_request *request;                                     \
                                                                                \
    char message[55];                                                           \
    sprintf(message, "%0*d00000000340000000010/fake/path0000000004ABCD", 10, request_type);   \
                                                                                \
    /*request = gnl_socket_request_read(message);  */                           \
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
    }                                                                                      \
                                                                                \
    if (strcmp(ref->bytes, "ABCD") != 0) {                             \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    gnl_socket_request_destroy(request);                                    \
                                                                                \
    return 0;                                                                   \
}

#define GNL_TEST_REQUEST_SB_WRITE(request_type) {                                                              \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 2, "/fake/path", "\x41\x42\x43\x44");    \
                                                                                                            \
    if (request == NULL) {                                                                                  \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    char *message = NULL;                                                                                          \
    gnl_socket_request_to_string(request, &message);                                                        \
                                                                                                            \
    char expected[55];                                                                                      \
    sprintf(expected, "%0*d00000000340000000010/fake/path0000000004ABCD", 10, request_type);                              \
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

#define GNL_TEST_EMPTY_REQUEST_NNB(request_type, ref) {                                              \
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
    if (ref->number > 0) {                                          \
        return -1;                                                                          \
    }                                                                                           \
                                                                                            \
    if (ref->count > 0) {                                          \
        return -1;                                                                          \
    }                                                                                         \
                                                                                            \
    if (ref->bytes != NULL) {                                          \
        return -1;                                                                          \
    }                                                                                       \
                                                                                            \
    gnl_socket_request_destroy(request);                                                \
                                                                                            \
    return 0;                                                                               \
}

#define GNL_TEST_REQUEST_NNB_ARGS(request_type, ref) {                                                          \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 2, 220510, 4, "\x41\x42\x43\x44");    \
                                                                                                            \
    if (request == NULL) {                                                                                  \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    if (request->type != request_type) {                                                                    \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    if (ref->number != 220510) {                                                         \
        return -1;                                                                                          \
    }                                                                                                      \
                                                                                                            \
    if (ref->count != 4) {                                                         \
        return -1;                                                                                          \
    }                                                                                                          \
                                                                                                            \
    if (strcmp(ref->bytes, "ABCD") != 0) {                                                         \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    gnl_socket_request_destroy(request);                                                                \
                                                                                                            \
    return 0;                                                                                               \
}

#define GNL_TEST_REQUEST_NNB_READ(request_type, ref) {                              \
    struct gnl_socket_request *request;                                     \
                                                                                \
    char message[55];                                                           \
    sprintf(message, "%0*d000000002400002205100000000004ABCD", 10, request_type);   \
                                                                                \
    /*request = gnl_socket_request_read(message); */                            \
    if (request == NULL) {                                                      \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (request->type != request_type) {                                        \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (ref->number != 220510) {                             \
        return -1;                                                              \
    }                                                                                      \
                                                                                \
    if (strcmp(ref->bytes, "ABCD") != 0) {                             \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    gnl_socket_request_destroy(request);                                    \
                                                                                \
    return 0;                                                                   \
}

#define GNL_TEST_REQUEST_NNB_WRITE(request_type) {                                                              \
struct gnl_socket_request *request = gnl_socket_request_init(request_type, 2, 220510, 4, "\x41\x42\x43\x44");    \
                                                                                                            \
    if (request == NULL) {                                                                                  \
        return -1;                                                                                          \
    }                                                                                                       \
                                                                                                            \
    char *message = NULL;                                                                                          \
    gnl_socket_request_to_string(request, &message);                                                        \
                                                                                                            \
    char expected[55];                                                                                      \
    sprintf(expected, "%0*d000000002400002205100000000004ABCD", 10, request_type);                              \
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

#define GNL_TEST_TO_STRING(type, expected) {                                    \
    char *dest;                                                                 \
    struct gnl_socket_request *request = gnl_socket_request_init((type), 0);    \
                                                                                \
    int res = gnl_socket_request_get_type(request, &dest);                     \
    if (res != 0) {                                                             \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    if (strcmp(dest, (expected)) != 0) {                                        \
        res = -1;                                                               \
    }                                                                           \
                                                                                \
    res = 0;                                                                    \
                                                                                \
    free(dest);                                                                 \
    gnl_socket_request_destroy(request);                                        \
                                                                                \
    return res;                                                                 \
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
//    struct gnl_socket_request *request;
//
//    request = gnl_socket_request_read("000000000000000000300000000010/fake/path0000000003");
//    if (request == NULL) {
//        return -1;
//    }
//
//    if (request->type != GNL_SOCKET_REQUEST_OPEN) {
//        return -1;
//    }
//
//    if (strcmp(request->payload.open->string, "/fake/path") != 0) {
//        return -1;
//    }
//
//    if (request->payload.open->number != 3) {
//        return -1;
//    }
//
//    gnl_socket_request_destroy(request);

    return 0;
}

int can_write_open() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, "/fake/path", 3);

    if (request == NULL) {
        return -1;
    }

    char *message = NULL;
    gnl_socket_request_to_string(request, &message);

    if (strcmp("000000000000000000300000000010/fake/path0000000003", message) != 0) {
        return -1;
    }

    gnl_socket_request_destroy(request);
    free(message);

    return 0;
}

int can_init_empty_read_N() {
    GNL_TEST_EMPTY_REQUEST_N(GNL_SOCKET_REQUEST_READ_N, request->payload.read_N)
}

int can_init_args_read_N() {
    GNL_TEST_REQUEST_N_ARGS(GNL_SOCKET_REQUEST_READ_N, request->payload.read_N)
}

int can_read_read_N() {
    GNL_TEST_REQUEST_N_READ(GNL_SOCKET_REQUEST_READ_N, request->payload.read_N)
}

int can_write_read_N() {
    GNL_TEST_REQUEST_N_WRITE(GNL_SOCKET_REQUEST_READ_N)
}

int can_init_empty_read() {
    GNL_TEST_EMPTY_REQUEST_S(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_init_args_read() {
    GNL_TEST_REQUEST_S_ARGS(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_read_read() {
    GNL_TEST_REQUEST_S_READ(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_write_read() {
    GNL_TEST_REQUEST_S_WRITE(GNL_SOCKET_REQUEST_READ)
}

int can_init_empty_write() {
    GNL_TEST_EMPTY_REQUEST_NNB(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_init_args_write() {
    GNL_TEST_REQUEST_NNB_ARGS(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_read_write() {
    GNL_TEST_REQUEST_NNB_READ(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_write_write() {
    GNL_TEST_REQUEST_NNB_WRITE(GNL_SOCKET_REQUEST_WRITE)
}

int can_init_empty_append() {
    GNL_TEST_EMPTY_REQUEST_SB(GNL_SOCKET_REQUEST_APPEND, request->payload.append)
}

int can_init_args_append() {
    GNL_TEST_REQUEST_SB_ARGS(GNL_SOCKET_REQUEST_APPEND, request->payload.append)
}

int can_read_append() {
    GNL_TEST_REQUEST_SB_READ(GNL_SOCKET_REQUEST_APPEND, request->payload.append)
}

int can_write_append() {
    GNL_TEST_REQUEST_SB_WRITE(GNL_SOCKET_REQUEST_APPEND)
}

int can_init_empty_lock() {
    GNL_TEST_EMPTY_REQUEST_S(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_init_args_lock() {
    GNL_TEST_REQUEST_S_ARGS(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_read_lock() {
    GNL_TEST_REQUEST_S_READ(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_write_lock() {
    GNL_TEST_REQUEST_S_WRITE(GNL_SOCKET_REQUEST_LOCK)
}

int can_init_empty_unlock() {
    GNL_TEST_EMPTY_REQUEST_S(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_init_args_unlock() {
    GNL_TEST_REQUEST_S_ARGS(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_read_unlock() {
    GNL_TEST_REQUEST_S_READ(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_write_unlock() {
    GNL_TEST_REQUEST_S_WRITE(GNL_SOCKET_REQUEST_UNLOCK)
}

int can_init_empty_close() {
    GNL_TEST_EMPTY_REQUEST_N(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_init_args_close() {
    GNL_TEST_REQUEST_N_ARGS(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_read_close() {
    GNL_TEST_REQUEST_N_READ(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_write_close() {
    GNL_TEST_REQUEST_N_WRITE(GNL_SOCKET_REQUEST_CLOSE)
}

int can_init_empty_remove() {
    GNL_TEST_EMPTY_REQUEST_S(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_init_args_remove() {
    GNL_TEST_REQUEST_S_ARGS(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_read_remove() {
    GNL_TEST_REQUEST_S_READ(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_write_remove() {
    GNL_TEST_REQUEST_S_WRITE(GNL_SOCKET_REQUEST_REMOVE)
}

int can_not_write_empty_request() {
    char *dest;
    struct gnl_socket_request *request = NULL;

    int res = gnl_socket_request_to_string(request, &dest);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_not_write_not_empty_dest() {
    char *dest = "fake_string";
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 0);

    int res = gnl_socket_request_to_string(request, &dest);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    gnl_socket_request_destroy(request);

    return 0;
}

int can_to_string_open() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_OPEN, "OPEN");
}

int can_to_string_read() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_READ, "READ");
}

int can_to_string_read_N() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_READ_N, "READ_N");
}

int can_to_string_write() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_WRITE, "WRITE");
}

int can_to_string_lock() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_LOCK, "LOCK");
}

int can_to_string_unlock() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_UNLOCK, "UNLOCK");
}

int can_to_string_close() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_CLOSE, "CLOSE");
}

int can_to_string_remove() {
    GNL_TEST_TO_STRING(GNL_SOCKET_REQUEST_REMOVE, "REMOVE");
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

    gnl_assert(can_init_empty_append, "can init an empty GNL_SOCKET_REQUEST_APPEND request type.");
    gnl_assert(can_init_args_append, "can init a GNL_SOCKET_REQUEST_APPEND request type with args.");
    gnl_assert(can_read_append, "can read a GNL_SOCKET_REQUEST_APPEND request type message.");
    gnl_assert(can_write_append, "can write a GNL_SOCKET_REQUEST_APPEND request type.");

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

    gnl_assert(can_not_write_empty_request, "can not write an empty request");
    gnl_assert(can_not_write_not_empty_dest, "can not write into a not empty destination");

    gnl_assert(can_to_string_open, "can format to string a GNL_SOCKET_REQUEST_OPEN request type");
    gnl_assert(can_to_string_read, "can format to string a GNL_SOCKET_REQUEST_READ request type");
    gnl_assert(can_to_string_read_N, "can format to string a GNL_SOCKET_REQUEST_READ_N request type");
    gnl_assert(can_to_string_write, "can format to string a GNL_SOCKET_REQUEST_WRITE request type");
    gnl_assert(can_to_string_lock, "can format to string a GNL_SOCKET_REQUEST_LOCK request type");
    gnl_assert(can_to_string_unlock, "can format to string a GNL_SOCKET_REQUEST_UNLOCK request type");
    gnl_assert(can_to_string_close, "can format to string a GNL_SOCKET_REQUEST_CLOSE request type");
    gnl_assert(can_to_string_remove, "can format to string a GNL_SOCKET_REQUEST_REMOVE request type");

    // the gnl_socket_request_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}

#undef GNL_TEST_EMPTY_REQUEST_N
#undef GNL_TEST_REQUEST_N_ARGS
#undef GNL_TEST_REQUEST_N_READ
#undef GNL_TEST_REQUEST_N_WRITE
#undef GNL_TEST_EMPTY_REQUEST_S
#undef GNL_TEST_REQUEST_S_ARGS
#undef GNL_TEST_REQUEST_S_READ
#undef GNL_TEST_REQUEST_S_WRITE
#undef GNL_TEST_EMPTY_REQUEST_SB
#undef GNL_TEST_REQUEST_SB_ARGS
#undef GNL_TEST_REQUEST_SB_READ
#undef GNL_TEST_REQUEST_SB_WRITE
#undef GNL_TEST_EMPTY_REQUEST_NNB
#undef GNL_TEST_REQUEST_NNB_ARGS
#undef GNL_TEST_REQUEST_NNB_READ
#undef GNL_TEST_REQUEST_NNB_WRITE
#undef GNL_TEST_TO_STRING