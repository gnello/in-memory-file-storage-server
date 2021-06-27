#include <stdio.h>
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

#define GNL_TEST_RESPONSE_N_READ(response_type, message_n, message) { \
    struct gnl_socket_response *response; \
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

#define GNL_TEST_RESPONSE_N_WRITE(response_type, message) { \
    struct gnl_socket_response *response = gnl_socket_response_init(response_type, 1, 3); \
\
    if (response == NULL) { \
        return -1; \
    } \
\
    char *actual = NULL; \
    gnl_socket_response_write(response, &actual); \
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

int can_init_empty_open() {
    GNL_TEST_RESPONSE_N_INIT(GNL_SOCKET_RESPONSE_OPEN);
}

int can_init_args_open() {
    GNL_TEST_RESPONSE_N_ARGS(GNL_SOCKET_RESPONSE_OPEN, response->payload.open);
}

int can_read_open() {
    GNL_TEST_RESPONSE_N_READ(GNL_SOCKET_RESPONSE_OPEN, response->payload.open, "000000000000000000100000000003")
}

int can_write_open() {
    GNL_TEST_RESPONSE_N_WRITE(GNL_SOCKET_RESPONSE_OPEN, "000000000000000000100000000003")
}
//
//int can_init_empty_read_N() {
//    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_READ_N, 0);
//
//    if (response == NULL) {
//        return -1;
//    }
//
//    if (response->type != GNL_SOCKET_RESPONSE_READ_N) {
//        return -1;
//    }
//
//    gnl_socket_response_destroy(response);
//
//    return 0;
//}
//
//int can_init_args_read_N() {
//    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_READ_N, 1, 15);
//
//    if (response == NULL) {
//        return -1;
//    }
//
//    if (response->type != GNL_SOCKET_RESPONSE_READ_N) {
//        return -1;
//    }
//
//    if (response->payload.read_N->number != 15) {
//        return -1;
//    }
//
//    gnl_socket_response_destroy(response);
//
//    return 0;
//}
//
//int can_read_read_N() {
//    struct gnl_socket_response *response;
//
//    response = gnl_socket_response_read("000000000200000000100000000015");
//    if (response == NULL) {
//        return -1;
//    }
//
//    if (response->type != GNL_SOCKET_RESPONSE_READ_N) {
//        return -1;
//    }
//
//    if (response->payload.read_N->number != 15) {
//        return -1;
//    }
//
//    gnl_socket_response_destroy(response);
//
//    return 0;
//}
//
//int can_write_read_N() {
//    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_READ_N, 1, 15);
//
//    if (response == NULL) {
//        return -1;
//    }
//
//    char *message = NULL;
//    gnl_socket_response_write(response, &message);
//
//    if (strcmp("000000000200000000100000000015", message) != 0) {
//        return -1;
//    }
//
//    gnl_socket_response_destroy(response);
//    free(message);
//
//    return 0;
//}
//
//int can_init_empty_read() {
//    GNL_TEST_EMPTY_RESPONSE_S(GNL_SOCKET_RESPONSE_READ, response->payload.read)
//}
//
//int can_init_args_read() {
//    GNL_TEST_RESPONSE_S_ARGS(GNL_SOCKET_RESPONSE_READ, response->payload.read)
//}
//
//int can_read_read() {
//    GNL_TEST_RESPONSE_S_READ(GNL_SOCKET_RESPONSE_READ, response->payload.read)
//}
//
//int can_write_read() {
//    GNL_TEST_RESPONSE_S_WRITE(GNL_SOCKET_RESPONSE_READ)
//}
//
//int can_init_empty_write() {
//    GNL_TEST_EMPTY_RESPONSE_SB(GNL_SOCKET_RESPONSE_WRITE, response->payload.write)
//}
//
//int can_init_args_write() {
//    GNL_TEST_RESPONSE_SB_ARGS(GNL_SOCKET_RESPONSE_WRITE, response->payload.write)
//}
//
//int can_read_write() {
//    GNL_TEST_RESPONSE_SB_READ(GNL_SOCKET_RESPONSE_WRITE, response->payload.write)
//}
//
//int can_write_write() {
//    GNL_TEST_RESPONSE_SB_WRITE(GNL_SOCKET_RESPONSE_WRITE)
//}
//
//int can_init_empty_append() {
//    GNL_TEST_EMPTY_RESPONSE_SB(GNL_SOCKET_RESPONSE_APPEND, response->payload.append)
//}
//
//int can_init_args_append() {
//    GNL_TEST_RESPONSE_SB_ARGS(GNL_SOCKET_RESPONSE_APPEND, response->payload.append)
//}
//
//int can_read_append() {
//    GNL_TEST_RESPONSE_SB_READ(GNL_SOCKET_RESPONSE_APPEND, response->payload.append)
//}
//
//int can_write_append() {
//    GNL_TEST_RESPONSE_SB_WRITE(GNL_SOCKET_RESPONSE_APPEND)
//}
//
//int can_init_empty_lock() {
//    GNL_TEST_EMPTY_RESPONSE_S(GNL_SOCKET_RESPONSE_LOCK, response->payload.lock)
//}
//
//int can_init_args_lock() {
//    GNL_TEST_RESPONSE_S_ARGS(GNL_SOCKET_RESPONSE_LOCK, response->payload.lock)
//}
//
//int can_read_lock() {
//    GNL_TEST_RESPONSE_S_READ(GNL_SOCKET_RESPONSE_LOCK, response->payload.lock)
//}
//
//int can_write_lock() {
//    GNL_TEST_RESPONSE_S_WRITE(GNL_SOCKET_RESPONSE_LOCK)
//}
//
//int can_init_empty_unlock() {
//    GNL_TEST_EMPTY_RESPONSE_S(GNL_SOCKET_RESPONSE_UNLOCK, response->payload.unlock)
//}
//
//int can_init_args_unlock() {
//    GNL_TEST_RESPONSE_S_ARGS(GNL_SOCKET_RESPONSE_UNLOCK, response->payload.unlock)
//}
//
//int can_read_unlock() {
//    GNL_TEST_RESPONSE_S_READ(GNL_SOCKET_RESPONSE_UNLOCK, response->payload.unlock)
//}
//
//int can_write_unlock() {
//    GNL_TEST_RESPONSE_S_WRITE(GNL_SOCKET_RESPONSE_UNLOCK)
//}
//
//int can_init_empty_close() {
//    GNL_TEST_EMPTY_RESPONSE_S(GNL_SOCKET_RESPONSE_CLOSE, response->payload.close)
//}
//
//int can_init_args_close() {
//    GNL_TEST_RESPONSE_S_ARGS(GNL_SOCKET_RESPONSE_CLOSE, response->payload.close)
//}
//
//int can_read_close() {
//    GNL_TEST_RESPONSE_S_READ(GNL_SOCKET_RESPONSE_CLOSE, response->payload.close)
//}
//
//int can_write_close() {
//    GNL_TEST_RESPONSE_S_WRITE(GNL_SOCKET_RESPONSE_CLOSE)
//}
//
//int can_init_empty_remove() {
//    GNL_TEST_EMPTY_RESPONSE_S(GNL_SOCKET_RESPONSE_REMOVE, response->payload.remove)
//}
//
//int can_init_args_remove() {
//    GNL_TEST_RESPONSE_S_ARGS(GNL_SOCKET_RESPONSE_REMOVE, response->payload.remove)
//}
//
//int can_read_remove() {
//    GNL_TEST_RESPONSE_S_READ(GNL_SOCKET_RESPONSE_REMOVE, response->payload.remove)
//}
//
//int can_write_remove() {
//    GNL_TEST_RESPONSE_S_WRITE(GNL_SOCKET_RESPONSE_REMOVE)
//}
//
//int can_not_write_empty_response() {
//    char *dest;
//    struct gnl_socket_response *response = NULL;
//
//    int res = gnl_socket_response_write(response, &dest);
//
//    if (res == 0) {
//        return -1;
//    }
//
//    if (errno != EINVAL) {
//        return -1;
//    }
//
//    return 0;
//}
//
//int can_not_write_not_empty_dest() {
//    char *dest = "fake_string";
//    struct gnl_socket_response *response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OPEN, 0);
//
//    int res = gnl_socket_response_write(response, &dest);
//
//    if (res == 0) {
//        return -1;
//    }
//
//    if (errno != EINVAL) {
//        return -1;
//    }
//
//    gnl_socket_response_destroy(response);
//
//    return 0;
//}
//
//int can_to_string_open() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_OPEN, "OPEN");
//}
//
//int can_to_string_read() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_READ, "READ");
//}
//
//int can_to_string_read_N() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_READ_N, "READ_N");
//}
//
//int can_to_string_write() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_WRITE, "WRITE");
//}
//
//int can_to_string_lock() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_LOCK, "LOCK");
//}
//
//int can_to_string_unlock() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_UNLOCK, "UNLOCK");
//}
//
//int can_to_string_close() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_CLOSE, "CLOSE");
//}
//
//int can_to_string_remove() {
//    GNL_TEST_TO_STRING(GNL_SOCKET_RESPONSE_REMOVE, "REMOVE");
//}

int main() {
    gnl_printf_yellow("> gnl_socket_response test:\n\n");

    gnl_assert(can_init_empty_open, "can init an empty GNL_SOCKET_RESPONSE_OPEN response type.");
    gnl_assert(can_init_args_open, "can init a GNL_SOCKET_RESPONSE_OPEN response type with args.");
    gnl_assert(can_read_open, "can read a GNL_SOCKET_RESPONSE_OPEN response type message.");
    gnl_assert(can_write_open, "can write a GNL_SOCKET_RESPONSE_OPEN response type.");

//    gnl_assert(can_init_empty_read, "can init an empty GNL_SOCKET_RESPONSE_READ response type.");
//    gnl_assert(can_init_args_read, "can init a GNL_SOCKET_RESPONSE_READ response type with args.");
//    gnl_assert(can_read_read, "can read a GNL_SOCKET_RESPONSE_READ response type message.");
//    gnl_assert(can_write_read, "can write a GNL_SOCKET_RESPONSE_READ response type.");
//
//    gnl_assert(can_init_empty_read_N, "can init an empty GNL_SOCKET_RESPONSE_READ_N response type.");
//    gnl_assert(can_init_args_read_N, "can init a GNL_SOCKET_RESPONSE_READ_N response type with args.");
//    gnl_assert(can_read_read_N, "can read a GNL_SOCKET_RESPONSE_READ_N response type message.");
//    gnl_assert(can_write_read_N, "can write a GNL_SOCKET_RESPONSE_READ_N response type.");
//
//    gnl_assert(can_init_empty_write, "can init an empty GNL_SOCKET_RESPONSE_WRITE response type.");
//    gnl_assert(can_init_args_write, "can init a GNL_SOCKET_RESPONSE_WRITE response type with args.");
//    gnl_assert(can_read_write, "can read a GNL_SOCKET_RESPONSE_WRITE response type message.");
//    gnl_assert(can_write_write, "can write a GNL_SOCKET_RESPONSE_WRITE response type.");
//
//    gnl_assert(can_init_empty_lock, "can init an empty GNL_SOCKET_RESPONSE_LOCK response type.");
//    gnl_assert(can_init_args_lock, "can init a GNL_SOCKET_RESPONSE_LOCK response type with args.");
//    gnl_assert(can_read_lock, "can read a GNL_SOCKET_RESPONSE_LOCK response type message.");
//    gnl_assert(can_write_lock, "can write a GNL_SOCKET_RESPONSE_LOCK response type.");
//
//    gnl_assert(can_init_empty_unlock, "can init an empty GNL_SOCKET_RESPONSE_UNLOCK response type.");
//    gnl_assert(can_init_args_unlock, "can init a GNL_SOCKET_RESPONSE_UNLOCK response type with args.");
//    gnl_assert(can_read_unlock, "can read a GNL_SOCKET_RESPONSE_UNLOCK response type message.");
//    gnl_assert(can_write_unlock, "can write a GNL_SOCKET_RESPONSE_UNLOCK response type.");
//
//    gnl_assert(can_init_empty_close, "can init an empty GNL_SOCKET_RESPONSE_CLOSE response type.");
//    gnl_assert(can_init_args_close, "can init a GNL_SOCKET_RESPONSE_CLOSE response type with args.");
//    gnl_assert(can_read_close, "can read a GNL_SOCKET_RESPONSE_CLOSE response type message.");
//    gnl_assert(can_write_close, "can write a GNL_SOCKET_RESPONSE_CLOSE response type.");
//
//    gnl_assert(can_init_empty_remove, "can init an empty GNL_SOCKET_RESPONSE_REMOVE response type.");
//    gnl_assert(can_init_args_remove, "can init a GNL_SOCKET_RESPONSE_REMOVE response type with args.");
//    gnl_assert(can_read_remove, "can read a GNL_SOCKET_RESPONSE_REMOVE response type message.");
//    gnl_assert(can_write_remove, "can write a GNL_SOCKET_RESPONSE_REMOVE response type.");
//
//    gnl_assert(can_not_write_empty_response, "can not write an empty response");
//    gnl_assert(can_not_write_not_empty_dest, "can not write into a not empty destination");
//
//    gnl_assert(can_to_string_open, "can format to string a GNL_SOCKET_RESPONSE_OPEN response type");
//    gnl_assert(can_to_string_read, "can format to string a GNL_SOCKET_RESPONSE_READ response type");
//    gnl_assert(can_to_string_read_N, "can format to string a GNL_SOCKET_RESPONSE_READ_N response type");
//    gnl_assert(can_to_string_write, "can format to string a GNL_SOCKET_RESPONSE_WRITE response type");
//    gnl_assert(can_to_string_lock, "can format to string a GNL_SOCKET_RESPONSE_LOCK response type");
//    gnl_assert(can_to_string_unlock, "can format to string a GNL_SOCKET_RESPONSE_UNLOCK response type");
//    gnl_assert(can_to_string_close, "can format to string a GNL_SOCKET_RESPONSE_CLOSE response type");
//    gnl_assert(can_to_string_remove, "can format to string a GNL_SOCKET_RESPONSE_REMOVE response type");

    // the gnl_socket_response_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}

#undef GNL_TEST_RESPONSE_N_INIT
#undef GNL_TEST_RESPONSE_N_ARGS
#undef GNL_TEST_RESPONSE_N_READ
#undef GNL_TEST_RESPONSE_N_WRITE