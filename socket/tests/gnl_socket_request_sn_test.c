#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_request_sn.c"

int can_init() {
    struct gnl_socket_request_sn *request_sn = gnl_socket_request_sn_init();

    if (request_sn == NULL) {
        return -1;
    }

    if (request_sn->string != NULL) {
        return -1;
    }

    if (request_sn->number > 0) {
        return -1;
    }

    gnl_socket_request_sn_destroy(request_sn);

    return 0;
}

int can_init_args() {
    struct gnl_socket_request_sn *request_sn = gnl_socket_request_sn_init_with_args("/fake/path", 1);

    if (strcmp(request_sn->string, "/fake/path") != 0) {
        return -1;
    }

    if (request_sn->number != 1) {
        return -1;
    }

    gnl_socket_request_sn_destroy(request_sn);

    return 0;
}

int can_build_message() {
    struct gnl_socket_request_sn *request_sn = gnl_socket_request_sn_init_with_args("/fake/path", 1);

    char *message;
    char *expected = "0000000010/fake/path0000000001";

    int res = gnl_socket_request_sn_write(*request_sn, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_socket_request_sn_destroy(request_sn);

    return 0;
}

int can_read_message() {
    struct gnl_socket_request_sn *request_sn = gnl_socket_request_sn_init();
    char *message = "0000000010/fake/path0000000001";

    int res = gnl_socket_request_sn_read(message, request_sn);

    if (res != 0) {
        return -1;
    }

    if (strcmp(request_sn->string, "/fake/path") != 0) {
        return -1;
    }

    if (request_sn->number != 1) {
        return -1;
    }

    gnl_socket_request_sn_destroy(request_sn);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_request_sn test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_socket_request_sn struct.");
    gnl_assert(can_init_args, "can init a gnl_socket_request_sn struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_socket_request_sn struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_socket_request_sn struct.");

    // the gnl_socket_request_sn_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}