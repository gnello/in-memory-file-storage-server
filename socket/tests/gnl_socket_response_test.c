#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_file_to_pointer.h>
#include "../src/gnl_socket_response.c"

#define GNL_TEST_RESPONSE_INIT(response_type) {                                                                         \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 0);                                  \
                                                                                                                        \
    if (response == NULL) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (response->type != response_type) {                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_response_destroy(response);                                                                              \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_RESPONSE_N_ARGS(response_type, message_n) {                                                            \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 1, 2);                               \
                                                                                                                        \
    if (response == NULL) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (response->type != response_type) {                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (message_n->number != 2) {                                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_response_destroy(response);                                                                              \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_RESPONSE_N_FROM_STRING(response_type, message_n) {                                                     \
    struct gnl_socket_response *response;                                                                               \
                                                                                                                        \
    char *message = "0000000003";                                                                                       \
                                                                                                                        \
    response = gnl_socket_response_from_string(message, response_type);                                                 \
    if (response == NULL) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (response->type != response_type) {                                                                              \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (message_n->number != 3) {                                                                                       \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_response_destroy(response);                                                                              \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_RESPONSE_N_TO_STRING(response_type) {                                                                  \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 1, 3);                               \
                                                                                                                        \
    char *message = "0000000003";                                                                                       \
                                                                                                                        \
    if (response == NULL) {                                                                                             \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    char *actual = NULL;                                                                                                \
    int res = gnl_socket_response_to_string(response, &actual);                                                         \
    if (res == -1) {                                                                                                    \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(message, actual) != 0) {                                                                                 \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    gnl_socket_response_destroy(response);                                                                              \
    free(actual);                                                                                                       \
                                                                                                                        \
    return 0;                                                                                                           \
}

#define GNL_TEST_TO_STRING(type, expected) {                                                                            \
    char *dest;                                                                                                         \
    struct gnl_socket_response *request = gnl_socket_response_init((type), 0);                                          \
                                                                                                                        \
    int res = gnl_socket_response_get_type(request, &dest);                                                             \
    if (res != 0) {                                                                                                     \
        return -1;                                                                                                      \
    }                                                                                                                   \
                                                                                                                        \
    if (strcmp(dest, (expected)) != 0) {                                                                                \
        res = -1;                                                                                                       \
    } else {                                                                                                            \
        res = 0;                                                                                                        \
    }                                                                                                                   \
                                                                                                                        \
    free(dest);                                                                                                         \
    gnl_socket_response_destroy(request);                                                                               \
                                                                                                                        \
    return res;                                                                                                         \
}

int can_init_empty_ok_file_list() {
    GNL_TEST_RESPONSE_INIT(GNL_SOCKET_RESPONSE_OK_FILE_LIST)
}

int can_from_string_ok_file_list() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_socket_response *response;

    char *message = calloc(35 + size, sizeof(char *));
    if (message == NULL) {
        return -1;
    }

    sprintf(message, "00000000010000000014./testfile.txt%0*ld", 10, size);

    memcpy(message + 45, content, size);

    response = gnl_socket_response_from_string(message, GNL_SOCKET_RESPONSE_OK_FILE_LIST);
    if (response == NULL) {
        return -1;
    }

    if (response->type != GNL_SOCKET_RESPONSE_OK_FILE_LIST) {
        return -1;
    }

    struct gnl_message_snb *file = gnl_socket_response_get_file(response);
    if (file == NULL) {
        return -1;
    }

    if (strcmp(file->string, "./testfile.txt") != 0) {
        return -1;
    }

    if (file->count != size) {
        return -1;
    }

    if (memcmp(file->bytes, content, size) != 0) {
        return -1;
    }

    free(file);
    free(message);
    gnl_socket_response_destroy(response);
    free(content);

    return 0;
}

int can_to_string_ok_file_list() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE_LIST, 0);
    if (response == NULL) {
        return -1;
    }

    void *test1 = "test1";
    void *test2 = "test2";
    void *test3 = "test3";
    void *test4 = "test4";
    void *test5 = "test5";

    gnl_socket_response_add_file(response, "test1", 5, test1);
    gnl_socket_response_add_file(response, "test2", 5, test2);
    gnl_socket_response_add_file(response, "./testfile.txt", size, content);
    gnl_socket_response_add_file(response, "test3", 5, test3);
    gnl_socket_response_add_file(response, "test4", 5, test4);
    gnl_socket_response_add_file(response, "test5", 5, test5);

    char *actual = NULL;
    res = gnl_socket_response_to_string(response, &actual);
    if (res == -1) {
        return -1;
    }

    char *message1 = "00000000060000000005test10000000005";

    if (strcmp(message1, actual) != 0) {
        return -1;
    }

    if (memcmp(actual + 36, test1, 5) != 0) {
        return -1;
    }

    char *message2 = "0000000005test20000000005";

    if (strcmp(message2, actual + 41) != 0) {
        return -1;
    }

    if (memcmp(actual + 67, test2, 5) != 0) {
        return -1;
    }

    char message_testfile[35];
    sprintf(message_testfile, "0000000014./testfile.txt%0*ld", 10, size);

    if (strcmp(message_testfile, actual + 72) != 0) {
        return -1;
    }

    if (memcmp(actual + 72 + 35, content, size) != 0) {
        return -1;
    }

    char *message3 = "0000000005test30000000005";

    if (strcmp(message3, actual + 107 + size) != 0) {
        return -1;
    }

    if (memcmp(actual + 133 + size, test3, 5) != 0) {
        return -1;
    }

    char *message4 = "0000000005test40000000005";

    if (strcmp(message4, actual + 138 + size) != 0) {
        return -1;
    }

    if (memcmp(actual + 164 + size, test4, 5) != 0) {
        return -1;
    }

    char *message5 = "0000000005test50000000005";

    if (strcmp(message5, actual + 169 + size) != 0) {
        return -1;
    }

    if (memcmp(actual + 195 + size, test5, 5) != 0) {
        return -1;
    }

    free(content);
    free(actual);
    gnl_socket_response_destroy(response);

    return 0;
}

int can_init_empty_ok_file() {
    GNL_TEST_RESPONSE_INIT(GNL_SOCKET_RESPONSE_OK_FILE);
}

int can_init_args_ok_file() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE, 3, "./testfile.txt", size, content);

    if (response == NULL) {
        return -1;
    }

    if (response->type != GNL_SOCKET_RESPONSE_OK_FILE) {
        return -1;
    }

    if (strcmp(response->payload.ok_file->string, "./testfile.txt") != 0) {
        return -1;
    }

    if (response->payload.ok_file->count != size) {
        return -1;
    }

    if (memcmp(response->payload.ok_file->bytes, content, size) != 0) {
        return -1;
    }

    free(content);
    gnl_socket_response_destroy(response);

    return 0;
}

int can_from_string_ok_file() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_socket_response *response;

    char *message = calloc(35 + size, sizeof(char *));
    if (message == NULL) {
        return -1;
    }

    sprintf(message, "0000000014./testfile.txt%0*ld", 10, size);

    memcpy(message + 35, content, size);

    response = gnl_socket_response_from_string(message, GNL_SOCKET_RESPONSE_OK_FILE);
    if (response == NULL) {
        return -1;
    }

    if (response->type != GNL_SOCKET_RESPONSE_OK_FILE) {
        return -1;
    }

    if (strcmp(response->payload.ok_file->string, "./testfile.txt") != 0) {
        return -1;
    }

    if (response->payload.ok_file->count != size) {
        return -1;
    }

    if (memcmp(response->payload.ok_file->bytes, content, size) != 0) {
        return -1;
    }

    free(message);
    gnl_socket_response_destroy(response);
    free(content);

    return 0;
}

int can_to_string_ok_file() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE, 3, "./testfile.txt", size, content);

    if (response == NULL) {
        return -1;
    }

    char message[35];
    sprintf(message, "0000000014./testfile.txt%0*ld", 10, size);

    char *actual = NULL;
    res = gnl_socket_response_to_string(response, &actual);
    if (res == -1) {
        return -1;
    }

    if (strcmp(message, actual) != 0) {
        return -1;
    }

    if (memcmp(actual + 35, content, size) != 0) {
        return -1;
    }

    gnl_socket_response_destroy(response);
    free(actual);
    free(content);

    return 0;
}

int can_init_empty_ok_fd() {
    GNL_TEST_RESPONSE_INIT(GNL_SOCKET_RESPONSE_OK_FD);
}

int can_init_args_ok_fd() {
    GNL_TEST_RESPONSE_N_ARGS(GNL_SOCKET_RESPONSE_OK_FD, response->payload.ok_fd);
}

int can_from_string_ok_fd() {
    GNL_TEST_RESPONSE_N_FROM_STRING(GNL_SOCKET_RESPONSE_OK_FD, response->payload.ok_fd)
}

int can_to_string_ok_fd() {
    GNL_TEST_RESPONSE_N_TO_STRING(GNL_SOCKET_RESPONSE_OK_FD)
}

int can_init_empty_ok() {
    GNL_TEST_RESPONSE_INIT(GNL_SOCKET_RESPONSE_OK);
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

int can_from_string_ok() {
    struct gnl_socket_response *response;

    char message[21];
    sprintf(message, "%0*d0000000000", 10, GNL_SOCKET_RESPONSE_OK);

    response = gnl_socket_response_from_string(message, GNL_SOCKET_RESPONSE_OK);
    if (response == NULL) {
        return -1;
    }

    if (response->type != GNL_SOCKET_RESPONSE_OK) {
        return -1;
    }

    gnl_socket_response_destroy(response);

    return 0;
}

int can_to_string_ok() {
    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);

    if (response == NULL) {
        return -1;
    }

    char *actual = NULL;
    int res = gnl_socket_response_to_string(response, &actual);
    if (res == -1) {
        return -1;
    }

    if (actual != NULL) {
        return -1;
    }

    if (res != 0) {
        return -1;
    }

    gnl_socket_response_destroy(response);
    free(actual);

    return 0;
}

int can_init_empty_error() {
    GNL_TEST_RESPONSE_INIT(GNL_SOCKET_RESPONSE_ERROR);
}

int can_init_args_error() {
    GNL_TEST_RESPONSE_N_ARGS(GNL_SOCKET_RESPONSE_ERROR, response->payload.error);
}

int can_from_string_error() {
    GNL_TEST_RESPONSE_N_FROM_STRING(GNL_SOCKET_RESPONSE_ERROR, response->payload.error)
}

int can_to_string_error() {
    GNL_TEST_RESPONSE_N_TO_STRING(GNL_SOCKET_RESPONSE_ERROR)
}

int can_not_write_empty_response() {
    char *dest;
    struct gnl_socket_response *response = NULL;

    int res = gnl_socket_response_to_string(response, &dest);

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
    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE_LIST, 0);

    int res = gnl_socket_response_to_string(response, &dest);

    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    gnl_socket_response_destroy(response);

    return 0;
}

int can_get_type_ok_file_list() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK_FILE_LIST, "OK_FILE_LIST");
}

int can_get_type_ok_file() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK_FILE, "OK_FILE");
}

int can_get_type_ok_fd() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK_FD, "OK_FD");
}

int can_get_type_ok() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OK, "OK");
}

int can_get_type_error() {
    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_ERROR, "ERROR");
}

int main() {
    gnl_printf_yellow("> gnl_socket_response test:\n\n");

    gnl_assert(can_init_empty_ok_file_list, "can init an empty GNL_SOCKET_RESPONSE_OK_FILE_LIST response type.");
    gnl_assert(can_from_string_ok_file_list, "can create from string a GNL_SOCKET_RESPONSE_OK_FILE_LIST response type message.");
    gnl_assert(can_to_string_ok_file_list, "can format to string a GNL_SOCKET_RESPONSE_OK_FILE_LIST response type.");

    gnl_assert(can_init_empty_ok_file, "can init an empty GNL_SOCKET_RESPONSE_OK_FILE response type.");
    gnl_assert(can_init_args_ok_file, "can init a GNL_SOCKET_RESPONSE_OK_FILE response type with args.");
    gnl_assert(can_from_string_ok_file, "can create from string a GNL_SOCKET_RESPONSE_OK_FILE response type message.");
    gnl_assert(can_to_string_ok_file, "can format to string a GNL_SOCKET_RESPONSE_OK_FILE response type.");

    gnl_assert(can_init_empty_ok_fd, "can init an empty GNL_SOCKET_RESPONSE_OK_FD response type.");
    gnl_assert(can_init_args_ok_fd, "can init a GNL_SOCKET_RESPONSE_OK_FD response type with args.");
    gnl_assert(can_from_string_ok_fd, "can create from string a GNL_SOCKET_RESPONSE_OK_FD response type message.");
    gnl_assert(can_to_string_ok_fd, "can format to string a GNL_SOCKET_RESPONSE_OK_FD response type.");

    gnl_assert(can_init_empty_ok, "can init an empty GNL_SOCKET_RESPONSE_OK response type.");
    gnl_assert(can_not_init_args_ok, "can not init a GNL_SOCKET_RESPONSE_OK response type with args.");
    gnl_assert(can_from_string_ok, "can create from string a GNL_SOCKET_RESPONSE_OK response type message.");
    gnl_assert(can_to_string_ok, "can format to string a GNL_SOCKET_RESPONSE_OK response type.");

    gnl_assert(can_init_empty_error, "can init an empty GNL_SOCKET_RESPONSE_ERROR response type.");
    gnl_assert(can_init_args_error, "can init a GNL_SOCKET_RESPONSE_ERROR response type with args.");
    gnl_assert(can_from_string_error, "can create from string a GNL_SOCKET_RESPONSE_ERROR response type message.");
    gnl_assert(can_to_string_error, "can format to string a GNL_SOCKET_RESPONSE_ERROR response type.");

    gnl_assert(can_not_write_empty_response, "can not write an empty response");
    gnl_assert(can_not_write_not_empty_dest, "can not write into a not empty destination");

    gnl_assert(can_get_type_ok_file_list, "can get the type string of a GNL_SOCKET_RESPONSE_OK_FILE_LIST response type");
    gnl_assert(can_get_type_ok_file, "can get the type string of a GNL_SOCKET_RESPONSE_OK_FILE response type");
    gnl_assert(can_get_type_ok_fd, "can get the type string of a GNL_SOCKET_RESPONSE_OK_FD response type");
    gnl_assert(can_get_type_ok, "can get the type string of a GNL_SOCKET_RESPONSE_OK response type");
    gnl_assert(can_get_type_error, "can get the type string of a GNL_SOCKET_RESPONSE_ERROR response type");

    // the gnl_socket_response_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}

#undef GNL_TEST_RESPONSE_INIT
#undef GNL_TEST_RESPONSE_N_ARGS
#undef GNL_TEST_RESPONSE_N_READ
#undef GNL_TEST_RESPONSE_N_WRITE
#undef GNL_TEST_TO_STRING