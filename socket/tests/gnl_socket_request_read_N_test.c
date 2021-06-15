#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_request_read_N.c"

int can_init() {
    struct gnl_socket_request_read_N *read_N = gnl_socket_request_read_N_init();

    if (read_N == NULL) {
        return -1;
    }

    if (read_N->N > 0) {
        return -1;
    }

    gnl_socket_request_read_N_destroy(read_N);

    return 0;
}

int can_init_args() {
    struct gnl_socket_request_read_N *read_N = gnl_socket_request_read_N_init_with_args(15);

    if (read_N->N != 15) {
        return -1;
    }

    gnl_socket_request_read_N_destroy(read_N);

    return 0;
}

int can_build_message() {
    struct gnl_socket_request_read_N *read_N = gnl_socket_request_read_N_init_with_args(15);

    char *message;
    char *expected = "0000000015";

    int res = gnl_socket_request_read_N_write(*read_N, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_socket_request_read_N_destroy(read_N);

    return 0;
}

int can_read_message() {
    struct gnl_socket_request_read_N *read_N = gnl_socket_request_read_N_init();
    char *message = "0000000015";

    int res = gnl_socket_request_read_N_read(message, read_N);

    if (res != 0) {
        return -1;
    }

    if (read_N->N != 15) {
        return -1;
    }

    gnl_socket_request_read_N_destroy(read_N);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_request_read_N test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_socket_request_read_N struct.");
    gnl_assert(can_init_args, "can init a gnl_socket_request_read_N struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_socket_request_read_N struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_socket_request_read_N struct.");

    // the gnl_socket_request_read_N_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}