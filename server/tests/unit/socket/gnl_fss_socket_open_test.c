#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/socket/gnl_fss_socket_open.c"

int can_init() {
    struct gnl_fss_socket_open *open = gnl_fss_socket_open_init();

    if (open == NULL) {
        return -1;
    }

    if (open->pathname != NULL) {
        return -1;
    }

    if (open->flags > 0) {
        return -1;
    }

    gnl_fss_socket_open_destroy(open);

    return 0;
}

int can_init_args() {
    struct gnl_fss_socket_open *open = gnl_fss_socket_open_init_with_args("/fake/path", 1);

    if (strcmp(open->pathname, "/fake/path") != 0) {
        return -1;
    }

    if (open->flags != 1) {
        return -1;
    }

    gnl_fss_socket_open_destroy(open);

    return 0;
}

int can_build_message() {
    struct gnl_fss_socket_open *open = gnl_fss_socket_open_init_with_args("/fake/path", 1);

    char *message;
    char *expected = "0000000010/fake/path1";

    int res = gnl_fss_socket_open_build_message(*open, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_fss_socket_open_destroy(open);

    return 0;
}

int can_read_message() {
    struct gnl_fss_socket_open *open = gnl_fss_socket_open_init();
    char *message = "0000000010/fake/path1";

    int res = gnl_fss_socket_open_read_message(message, open);

    if (res != 0) {
        return -1;
    }

    if (strcmp(open->pathname, "/fake/path") != 0) {
        return -1;
    }

    if (open->flags != 1) {
        return -1;
    }

    gnl_fss_socket_open_destroy(open);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_fss_socket_open test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_fss_socket_open struct.");
    gnl_assert(can_init_args, "can init a gnl_fss_socket_open struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_fss_socket_open struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_fss_socket_open struct.");

    // the gnl_fss_socket_open_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}