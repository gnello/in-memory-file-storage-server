#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_file_to_pointer.h>
#include "../src/gnl_socket_request.c"

#define GNL_TEST_EMPTY_REQUEST_N(request_type, ref) {                                                                   \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 0);                                      \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_N_ARGS(request_type, ref) {                                                                    \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, 15);                                  \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->number != 15) {                                                                                            \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_N_FROM_STRING(request_type, ref) {                                                             \
    struct gnl_socket_request *request;                                                                                 \
                                                                                                                        \
    char *message = "0000000015";                                                                                       \
                                                                                                                        \
    request = gnl_socket_request_from_string(message, request_type);                                                    \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->number != 15) {                                                                                            \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_N_TO_STRING(request_type) {                                                                    \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, 15);                                  \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *message = NULL;                                                                                               \
    int res = gnl_socket_request_to_string(request, &message);                                                          \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *expected = "0000000015";                                                                                      \
    if (strcmp(expected, message) != 0) {                                                                               \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
    free(message);                                                                                                      \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_EMPTY_REQUEST_S(request_type, ref) {                                                                   \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 0);                                      \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->string != NULL) {                                                                                          \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_S_ARGS(request_type, ref) {                                                                    \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, "/fake/path");                        \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(ref->string, "/fake/path") != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_S_FROM_STRING(request_type, ref) {                                                             \
    struct gnl_socket_request *request;                                                                                 \
                                                                                                                        \
    char *message = "0000000010/fake/path";                                                                             \
                                                                                                                        \
    request = gnl_socket_request_from_string(message, request_type);                                                    \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(ref->string, "/fake/path") != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_S_TO_STRING(request_type) {                                                                    \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 1, "/fake/path");                        \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *message = NULL;                                                                                               \
    int res = gnl_socket_request_to_string(request, &message);                                                          \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *expected = "0000000010/fake/path";                                                                            \
                                                                                                                        \
    if (strcmp(expected, message) != 0) {                                                                               \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
    free(message);                                                                                                      \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_EMPTY_REQUEST_SNB(request_type, ref) {                                                                 \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 0);                                      \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->string != NULL) {                                                                                          \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->count != 0) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->bytes != NULL) {                                                                                           \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_SNB_ARGS(request_type, ref) {                                                                  \
    int res;                                                                                                            \
    long size;                                                                                                          \
    char *content = NULL;                                                                                               \
                                                                                                                        \
    res = gnl_file_to_pointer("./testfile.txt", &content, &size);                                                       \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 3, "/fake/path", size, content);         \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(ref->string, "/fake/path") != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (memcmp(ref->bytes, content, size) != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    free(content);                                                                                                      \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_SNB_FROM_STRING(request_type, ref) {                                                           \
    struct gnl_socket_request *request;                                                                                 \
                                                                                                                        \
    int res;                                                                                                            \
    long size;                                                                                                          \
    char *content = NULL;                                                                                               \
                                                                                                                        \
    res = gnl_file_to_pointer("./testfile.txt", &content, &size);                                                       \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *message = calloc(31 + size, sizeof(char *));                                                                  \
    if (message == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    sprintf(message, "0000000010/fake/path%0*ld", 10, size);                                                            \
                                                                                                                        \
    memcpy(message + 31, content, size);                                                                                \
                                                                                                                        \
    request = gnl_socket_request_from_string(message, request_type);                                                    \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(ref->string, "/fake/path") != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->count != size) {                                                                                           \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (memcmp(ref->bytes, content, size) != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    free(message);                                                                                                      \
    free(content);                                                                                                      \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_SNB_TO_STRING(request_type) {                                                                  \
    int res;                                                                                                            \
    long size;                                                                                                          \
    char *content = NULL;                                                                                               \
                                                                                                                        \
    res = gnl_file_to_pointer("./testfile.txt", &content, &size);                                                       \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 3, "/fake/path", size, content);         \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *expected = calloc(31 + size, sizeof(char *));                                                                 \
    if (expected == NULL) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    sprintf(expected, "0000000010/fake/path%0*ld", 10, size);                                                           \
                                                                                                                        \
    memcpy(expected + 31, content, size);                                                                               \
                                                                                                                        \
    char *message = NULL;                                                                                               \
    res = gnl_socket_request_to_string(request, &message);                                                              \
                                                                                                                        \
    if (res != (strlen(expected) + 1) + size) {                                                                         \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(message, expected) != 0) {                                                                               \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (memcmp(message + 31, content, size) != 0) {                                                                     \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    free(content);                                                                                                      \
    free(message);                                                                                                      \
    free(expected);                                                                                                     \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_EMPTY_REQUEST_NNB(request_type, ref) {                                                                 \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 0);                                      \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->number != 0) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->count != 0) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->bytes != NULL) {                                                                                           \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_NNB_ARGS(request_type, ref) {                                                                  \
    int res;                                                                                                            \
    long size;                                                                                                          \
    char *content = NULL;                                                                                               \
                                                                                                                        \
    res = gnl_file_to_pointer("./testfile.txt", &content, &size);                                                       \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 3, 220510, size, content);               \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->number != 220510) {                                                                                        \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->count != size) {                                                                                           \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (memcmp(ref->bytes, content, size) != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    free(content);                                                                                                      \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_NNB_FROM_STRING(request_type, ref) {                                                           \
    struct gnl_socket_request *request;                                                                                 \
                                                                                                                        \
    int res;                                                                                                            \
    long size;                                                                                                          \
    char *content = NULL;                                                                                               \
                                                                                                                        \
    res = gnl_file_to_pointer("./testfile.txt", &content, &size);                                                       \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *message = calloc(21 + size, sizeof(char *));                                                                  \
    if (message == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    sprintf(message, "0000220510%0*ld", 10, size);                                                                      \
                                                                                                                        \
    memcpy(message + 21, content, size);                                                                                \
                                                                                                                        \
    request = gnl_socket_request_from_string(message, request_type);                                                    \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (request->type != request_type) {                                                                                \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->number != 220510) {                                                                                        \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (ref->count != size) {                                                                                           \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (memcmp(ref->bytes, content, size) != 0) {                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    free(message);                                                                                                      \
    free(content);                                                                                                      \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_REQUEST_NNB_TO_STRING(request_type) {                                                                  \
    int res;                                                                                                            \
    long size;                                                                                                          \
    char *content = NULL;                                                                                               \
                                                                                                                        \
    res = gnl_file_to_pointer("./testfile.txt", &content, &size);                                                       \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    struct gnl_socket_request *request = gnl_socket_request_init(request_type, 3, 220510, size, content);               \
                                                                                                                        \
    if (request == NULL) {                                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *expected = calloc(21 + size, sizeof(char *));                                                                 \
    if (expected == NULL) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    sprintf(expected, "0000220510%0*ld", 10, size);                                                                     \
                                                                                                                        \
    memcpy(expected + 21, content, size);                                                                               \
                                                                                                                        \
    char *message = NULL;                                                                                               \
    res = gnl_socket_request_to_string(request, &message);                                                              \
                                                                                                                        \
    if (res != (strlen(expected) + 1) + size) {                                                                         \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(message, expected) != 0) {                                                                               \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (memcmp(message + 21, content, size) != 0) {                                                                     \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    free(content);                                                                                                      \
    free(message);                                                                                                      \
    free(expected);                                                                                                     \
                                                                                                                        \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_GET_TYPE(type, expected) {                                                                             \
    char *dest;                                                                                                         \
    struct gnl_socket_request *request = gnl_socket_request_init((type), 0);                                            \
                                                                                                                        \
    int res = gnl_socket_request_get_type(request, &dest);                                                              \
    if (res != 0) {                                                                                                     \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(dest, (expected)) != 0) {                                                                                \
        res = -1;                                                                                                       \
    }                                                                                                                   \
                                                                                                                        \
    res = 0;                                                                                                            \
                                                                                                                        \
    free(dest);                                                                                                         \
    gnl_socket_request_destroy(request);                                                                                \
                                                                                                                        \
    return res;                                                                                                         \
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

int can_from_string_open() {
    struct gnl_socket_request *request;

    request = gnl_socket_request_from_string("0000000010/fake/path0000000003", GNL_SOCKET_REQUEST_OPEN);
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

int can_to_string_open() {
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, "/fake/path", 3);

    if (request == NULL) {
        return -1;
    }

    char *message = NULL;
    gnl_socket_request_to_string(request, &message);

    if (strcmp("0000000010/fake/path0000000003", message) != 0) {
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

int can_from_string_read_N() {
    GNL_TEST_REQUEST_N_FROM_STRING(GNL_SOCKET_REQUEST_READ_N, request->payload.read_N)
}

int can_to_string_read_N() {
    GNL_TEST_REQUEST_N_TO_STRING(GNL_SOCKET_REQUEST_READ_N)
}

int can_init_empty_read() {
    GNL_TEST_EMPTY_REQUEST_N(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_init_args_read() {
    GNL_TEST_REQUEST_N_ARGS(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_from_string_read() {
    GNL_TEST_REQUEST_N_FROM_STRING(GNL_SOCKET_REQUEST_READ, request->payload.read)
}

int can_to_string_read() {
    GNL_TEST_REQUEST_N_TO_STRING(GNL_SOCKET_REQUEST_READ)
}

int can_init_empty_write() {
    GNL_TEST_EMPTY_REQUEST_NNB(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_init_args_write() {
    GNL_TEST_REQUEST_NNB_ARGS(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_from_string_write() {
    GNL_TEST_REQUEST_NNB_FROM_STRING(GNL_SOCKET_REQUEST_WRITE, request->payload.write)
}

int can_to_string_write() {
    GNL_TEST_REQUEST_NNB_TO_STRING(GNL_SOCKET_REQUEST_WRITE)
}

int can_init_empty_append() {
    GNL_TEST_EMPTY_REQUEST_SNB(GNL_SOCKET_REQUEST_APPEND, request->payload.append)
}

int can_init_args_append() {
    GNL_TEST_REQUEST_SNB_ARGS(GNL_SOCKET_REQUEST_APPEND, request->payload.append)
}

int can_from_string_append() {
    GNL_TEST_REQUEST_SNB_FROM_STRING(GNL_SOCKET_REQUEST_APPEND, request->payload.append)
}

int can_to_string_append() {
    GNL_TEST_REQUEST_SNB_TO_STRING(GNL_SOCKET_REQUEST_APPEND)
}

int can_init_empty_lock() {
    GNL_TEST_EMPTY_REQUEST_N(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_init_args_lock() {
    GNL_TEST_REQUEST_N_ARGS(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_from_string_lock() {
    GNL_TEST_REQUEST_N_FROM_STRING(GNL_SOCKET_REQUEST_LOCK, request->payload.lock)
}

int can_to_string_lock() {
    GNL_TEST_REQUEST_N_TO_STRING(GNL_SOCKET_REQUEST_LOCK)
}

int can_init_empty_unlock() {
    GNL_TEST_EMPTY_REQUEST_N(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_init_args_unlock() {
    GNL_TEST_REQUEST_N_ARGS(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_from_string_unlock() {
    GNL_TEST_REQUEST_N_FROM_STRING(GNL_SOCKET_REQUEST_UNLOCK, request->payload.unlock)
}

int can_to_string_unlock() {
    GNL_TEST_REQUEST_N_TO_STRING(GNL_SOCKET_REQUEST_UNLOCK)
}

int can_init_empty_close() {
    GNL_TEST_EMPTY_REQUEST_N(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_init_args_close() {
    GNL_TEST_REQUEST_N_ARGS(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_from_string_close() {
    GNL_TEST_REQUEST_N_FROM_STRING(GNL_SOCKET_REQUEST_CLOSE, request->payload.close)
}

int can_to_string_close() {
    GNL_TEST_REQUEST_N_TO_STRING(GNL_SOCKET_REQUEST_CLOSE)
}

int can_init_empty_remove() {
    GNL_TEST_EMPTY_REQUEST_S(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_init_args_remove() {
    GNL_TEST_REQUEST_S_ARGS(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_from_string_remove() {
    GNL_TEST_REQUEST_S_FROM_STRING(GNL_SOCKET_REQUEST_REMOVE, request->payload.remove)
}

int can_to_string_remove() {
    GNL_TEST_REQUEST_S_TO_STRING(GNL_SOCKET_REQUEST_REMOVE)
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

int can_get_type_open() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_OPEN, "OPEN");
}

int can_get_type_read() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_READ, "READ");
}

int can_get_type_read_N() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_READ_N, "READ_N");
}

int can_get_type_write() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_WRITE, "WRITE");
}

int can_get_type_lock() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_LOCK, "LOCK");
}

int can_get_type_unlock() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_UNLOCK, "UNLOCK");
}

int can_get_type_close() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_CLOSE, "CLOSE");
}

int can_get_type_remove() {
    GNL_TEST_GET_TYPE(GNL_SOCKET_REQUEST_REMOVE, "REMOVE");
}

int main() {
    gnl_printf_yellow("> gnl_socket_request test:\n\n");

    gnl_assert(can_init_empty_open, "can init an empty GNL_SOCKET_REQUEST_OPEN request type.");
    gnl_assert(can_init_args_open, "can init a GNL_SOCKET_REQUEST_OPEN request type with args.");
    gnl_assert(can_from_string_open, "can create from string a GNL_SOCKET_REQUEST_OPEN request type message.");
    gnl_assert(can_to_string_open, "can format to string a GNL_SOCKET_REQUEST_OPEN request type.");

    gnl_assert(can_init_empty_read, "can init an empty GNL_SOCKET_REQUEST_READ request type.");
    gnl_assert(can_init_args_read, "can init a GNL_SOCKET_REQUEST_READ request type with args.");
    gnl_assert(can_from_string_read, "can create from string a GNL_SOCKET_REQUEST_READ request type message.");
    gnl_assert(can_to_string_read, "can format to string a GNL_SOCKET_REQUEST_READ request type.");

    gnl_assert(can_init_empty_read_N, "can init an empty GNL_SOCKET_REQUEST_READ_N request type.");
    gnl_assert(can_init_args_read_N, "can init a GNL_SOCKET_REQUEST_READ_N request type with args.");
    gnl_assert(can_from_string_read_N, "can create from string a GNL_SOCKET_REQUEST_READ_N request type message.");
    gnl_assert(can_to_string_read_N, "can format to string a GNL_SOCKET_REQUEST_READ_N request type.");

    gnl_assert(can_init_empty_write, "can init an empty GNL_SOCKET_REQUEST_WRITE request type.");
    gnl_assert(can_init_args_write, "can init a GNL_SOCKET_REQUEST_WRITE request type with args.");
    gnl_assert(can_from_string_write, "can create from string a GNL_SOCKET_REQUEST_WRITE request type message.");
    gnl_assert(can_to_string_write, "can format to string a GNL_SOCKET_REQUEST_WRITE request type.");

    gnl_assert(can_init_empty_append, "can init an empty GNL_SOCKET_REQUEST_APPEND request type.");
    gnl_assert(can_init_args_append, "can init a GNL_SOCKET_REQUEST_APPEND request type with args.");
    gnl_assert(can_from_string_append, "can create from string a GNL_SOCKET_REQUEST_APPEND request type message.");
    gnl_assert(can_to_string_append, "can format to string a GNL_SOCKET_REQUEST_APPEND request type.");

    gnl_assert(can_init_empty_lock, "can init an empty GNL_SOCKET_REQUEST_LOCK request type.");
    gnl_assert(can_init_args_lock, "can init a GNL_SOCKET_REQUEST_LOCK request type with args.");
    gnl_assert(can_from_string_lock, "can create from string a GNL_SOCKET_REQUEST_LOCK request type message.");
    gnl_assert(can_to_string_lock, "can format to string a GNL_SOCKET_REQUEST_LOCK request type.");

    gnl_assert(can_init_empty_unlock, "can init an empty GNL_SOCKET_REQUEST_UNLOCK request type.");
    gnl_assert(can_init_args_unlock, "can init a GNL_SOCKET_REQUEST_UNLOCK request type with args.");
    gnl_assert(can_from_string_unlock, "can create from string a GNL_SOCKET_REQUEST_UNLOCK request type message.");
    gnl_assert(can_to_string_unlock, "can format to string a GNL_SOCKET_REQUEST_UNLOCK request type.");

    gnl_assert(can_init_empty_close, "can init an empty GNL_SOCKET_REQUEST_CLOSE request type.");
    gnl_assert(can_init_args_close, "can init a GNL_SOCKET_REQUEST_CLOSE request type with args.");
    gnl_assert(can_from_string_close, "can create from string a GNL_SOCKET_REQUEST_CLOSE request type message.");
    gnl_assert(can_to_string_close, "can format to string a GNL_SOCKET_REQUEST_CLOSE request type.");

    gnl_assert(can_init_empty_remove, "can init an empty GNL_SOCKET_REQUEST_REMOVE request type.");
    gnl_assert(can_init_args_remove, "can init a GNL_SOCKET_REQUEST_REMOVE request type with args.");
    gnl_assert(can_from_string_remove, "can create from string a GNL_SOCKET_REQUEST_REMOVE request type message.");
    gnl_assert(can_to_string_remove, "can format to string a GNL_SOCKET_REQUEST_REMOVE request type.");

    gnl_assert(can_not_write_empty_request, "can not write an empty request");
    gnl_assert(can_not_write_not_empty_dest, "can not write into a not empty destination");

    gnl_assert(can_get_type_open, "can get the type string of a GNL_SOCKET_REQUEST_OPEN request type");
    gnl_assert(can_get_type_read, "can get the type string of a GNL_SOCKET_REQUEST_READ request type");
    gnl_assert(can_get_type_read_N, "can get the type string of a GNL_SOCKET_REQUEST_READ_N request type");
    gnl_assert(can_get_type_write, "can get the type string of a GNL_SOCKET_REQUEST_WRITE request type");
    gnl_assert(can_get_type_lock, "can get the type string of a GNL_SOCKET_REQUEST_LOCK request type");
    gnl_assert(can_get_type_unlock, "can get the type string of a GNL_SOCKET_REQUEST_UNLOCK request type");
    gnl_assert(can_get_type_close, "can get the type string of a GNL_SOCKET_REQUEST_CLOSE request type");
    gnl_assert(can_get_type_remove, "can get the type string of a GNL_SOCKET_REQUEST_REMOVE request type");

    // the gnl_socket_request_destroy method is implicitly tested in every assertion

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
#undef GNL_TEST_EMPTY_REQUEST_SNB
#undef GNL_TEST_REQUEST_SNB_ARGS
#undef GNL_TEST_REQUEST_SNB_READ
#undef GNL_TEST_REQUEST_SNB_WRITE
#undef GNL_TEST_EMPTY_REQUEST_NNB
#undef GNL_TEST_REQUEST_NNB_ARGS
#undef GNL_TEST_REQUEST_NNB_READ
#undef GNL_TEST_REQUEST_NNB_WRITE
#undef GNL_TEST_GET_TYPE