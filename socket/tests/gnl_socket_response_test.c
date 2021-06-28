#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_response.c"

#define GNL_TEST_RESPONSE_N_INIT(response_type) { \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 0); \
\
    if (response == NULL) { \
        return -1; \
    } \
\
    if (response->type != response_type) { \
        return -1; \
    } \
\
    gnl_socket_response_destroy(response); \
\
    return 0; \
}

#define GNL_TEST_RESPONSE_N_ARGS(response_type, message_n) { \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 1, 2); \
\
    if (response == NULL) { \
        return -1; \
    } \
\
    if (response->type != response_type) { \
        return -1; \
    } \
\
    if (message_n->number != 2) { \
        return -1; \
    } \
\
    gnl_socket_response_destroy(response); \
\
    return 0; \
}

#define GNL_TEST_RESPONSE_N_READ(response_type, message_n) { \
    struct gnl_socket_response *response;                    \
                                                             \
    char message[31]; \
    sprintf(message, "%0*d00000000100000000003", 10, response_type); \
\
    response = gnl_socket_response_read(message); \
    if (response == NULL) { \
        return -1; \
    } \
\
    if (response->type != response_type) { \
        return -1; \
    } \
\
    if (message_n->number != 3) { \
        return -1; \
    } \
\
    gnl_socket_response_destroy(response); \
\
    return 0; \
}

#define GNL_TEST_RESPONSE_N_WRITE(response_type) { \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 1, 3); \
                                                   \
    char message[31]; \
    sprintf(message, "%0*d00000000100000000003", 10, response_type); \
\
    if (response == NULL) { \
        return -1; \
    } \
\
    char *actual = NULL; \
    int res = gnl_socket_response_write(response, &actual);                               \
    if (res == -1) {                               \
        return -1;                                      \
    }                                              \
                                                   \
    if (strcmp(message, actual) != 0) { \
        return -1; \
    } \
\
    gnl_socket_response_destroy(response); \
    free(actual); \
\
    return 0; \
}

#define GNL_TEST_TO_STRING(type, expected) {                                    \
    char *dest;                                                                 \
    struct gnl_socket_response *request = gnl_socket_response_init((type), 0);    \
                                                                                \
    int res = gnl_socket_response_to_string(request, &dest);                     \
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
    gnl_socket_response_destroy(request);                                        \
                                                                                \
    return res;                                                                 \
}

int can_init_empty_ok_evicted() {
    GNL_TEST_RESPONSE_N_INIT(GNL_SOCKET_RESPONSE_OK_EVICTED);
}

int can_init_args_ok_evicted() {
    GNL_TEST_RESPONSE_N_ARGS(GNL_SOCKET_RESPONSE_OK_EVICTED, response->payload.ok_evicted);
}

int can_read_ok_evicted() {
    GNL_TEST_RESPONSE_N_READ(GNL_SOCKET_RESPONSE_OK_EVICTED, response->payload.ok_evicted)
}

int can_write_ok_evicted() {
    GNL_TEST_RESPONSE_N_WRITE(GNL_SOCKET_RESPONSE_OK_EVICTED)
}

int can_init_empty_ok() {
    GNL_TEST_RESPONSE_N_INIT(GNL_SOCKET_RESPONSE_OK);
}

int can_not_init_args_ok() {
    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 1, 2);

    if (response != NULL) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_read_ok() {
    struct gnl_socket_response *response;

    char message[21];
    sprintf(message, "%0*d0000000000", 10, GNL_SOCKET_RESPONSE_OK);

    response = gnl_socket_response_read(message);
    if (response == NULL) {
        return -1;
    }

    if (response->type != GNL_SOCKET_RESPONSE_OK) {
        return -1;
    }

    gnl_socket_response_destroy(response);

    return 0;
}

int can_write_ok() {
    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);

    if (response == NULL) {
        return -1;
    }

    char message[21];
    sprintf(message, "%0*d0000000000", 10, GNL_SOCKET_RESPONSE_OK);

    char *actual = NULL;
    int res = gnl_socket_response_write(response, &actual);
    if (res == -1) {
        return -1;
    }

    if (strcmp(message, actual) != 0) {
        return -1;
    }

    gnl_socket_response_destroy(response);
    free(actual);

    return 0;
}

int can_init_empty_error() {
    GNL_TEST_RESPONSE_N_INIT(GNL_SOCKET_RESPONSE_ERROR);
}

int can_init_args_error() {
    GNL_TEST_RESPONSE_N_ARGS(GNL_SOCKET_RESPONSE_ERROR, response->payload.error);
}

int can_read_error() {
    GNL_TEST_RESPONSE_N_READ(GNL_SOCKET_RESPONSE_ERROR, response->payload.error)
}

int can_write_error() {
    GNL_TEST_RESPONSE_N_WRITE(GNL_SOCKET_RESPONSE_ERROR)
}

int can_not_write_empty_response() {
    char *dest;
    struct gnl_socket_response *response = NULL;

    int res = gnl_socket_response_write(response, &dest);

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
    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_EVICTED, 0);

    int res = gnl_socket_response_write(response, &dest);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    gnl_socket_response_destroy(response);

    return 0;
}

int can_to_string_ok_evicted() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK_EVICTED, "OK_EVICTED");
}

int can_to_string_ok_file() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK_FILE, "OK_FILE");
}

int can_to_string_ok() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK, "OK");
}

int can_to_string_error() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_ERROR, "ERROR");
}

int main() {
    gnl_printf_yellow("> gnl_socket_response test:\n\n");

    gnl_assert(can_init_empty_ok_evicted, "can init an empty GNL_SOCKET_RESPONSE_OK_EVICTED response type.");
    gnl_assert(can_init_args_ok_evicted, "can init a GNL_SOCKET_RESPONSE_OK_EVICTED response type with args.");
    gnl_assert(can_read_ok_evicted, "can read a GNL_SOCKET_RESPONSE_OK_EVICTED response type message.");
    gnl_assert(can_write_ok_evicted, "can write a GNL_SOCKET_RESPONSE_OK_EVICTED response type.");

    gnl_assert(can_init_empty_ok, "can init an empty GNL_SOCKET_RESPONSE_OK response type.");
    gnl_assert(can_not_init_args_ok, "can not init a GNL_SOCKET_RESPONSE_OK response type with args.");
    gnl_assert(can_read_ok, "can read a GNL_SOCKET_RESPONSE_OK response type message.");
    gnl_assert(can_write_ok, "can write a GNL_SOCKET_RESPONSE_OK response type.");

    gnl_assert(can_init_empty_error, "can init an empty GNL_SOCKET_RESPONSE_ERROR response type.");
    gnl_assert(can_init_args_error, "can init a GNL_SOCKET_RESPONSE_ERROR response type with args.");
    gnl_assert(can_read_error, "can read a GNL_SOCKET_RESPONSE_ERROR response type message.");
    gnl_assert(can_write_error, "can write a GNL_SOCKET_RESPONSE_ERROR response type.");

    gnl_assert(can_not_write_empty_response, "can not write an empty response");
    gnl_assert(can_not_write_not_empty_dest, "can not write into a not empty destination");

    gnl_assert(can_to_string_ok_evicted, "can format to string a GNL_SOCKET_RESPONSE_OK_EVICTED response type");
    //gnl_assert(can_to_string_ok_file, "can format to string a GNL_SOCKET_RESPONSE_OK_FILE response type");
    gnl_assert(can_to_string_ok, "can format to string a GNL_SOCKET_RESPONSE_OK response type");
    gnl_assert(can_to_string_error, "can format to string a GNL_SOCKET_RESPONSE_ERROR response type");

    // the gnl_socket_response_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}

#undef GNL_TEST_RESPONSE_N_INIT
#undef GNL_TEST_RESPONSE_N_ARGS
#undef GNL_TEST_RESPONSE_N_READ
#undef GNL_TEST_RESPONSE_N_WRITE
#undef GNL_TEST_TO_STRING