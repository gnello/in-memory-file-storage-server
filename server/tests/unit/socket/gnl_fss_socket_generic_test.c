#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/socket/gnl_fss_socket_generic.c"

int can_init() {
    struct gnl_fss_socket_generic *generic = gnl_fss_socket_generic_init();

    if (generic == NULL) {
        return -1;
    }

    if (generic->pathname != NULL) {
        return -1;
    }

    gnl_fss_socket_generic_destroy(generic);

    return 0;
}

int can_init_args() {
    struct gnl_fss_socket_generic *generic = gnl_fss_socket_generic_init_with_args("/fake/path");

    if (strcmp(generic->pathname, "/fake/path") != 0) {
        return -1;
    }

    gnl_fss_socket_generic_destroy(generic);

    return 0;
}

int can_build_message() {
    struct gnl_fss_socket_generic *generic = gnl_fss_socket_generic_init_with_args("/fake/path");

    char *message;
    char *expected = "0000000010/fake/path";

    int res = gnl_fss_socket_generic_build_message(*generic, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_fss_socket_generic_destroy(generic);

    return 0;
}

int can_read_message() {
    struct gnl_fss_socket_generic *generic = gnl_fss_socket_generic_init();
    char *message = "0000000010/fake/path";

    int res = gnl_fss_socket_generic_read_message(message, generic);

    if (res != 0) {
        return -1;
    }

    if (strcmp(generic->pathname, "/fake/path") != 0) {
        return -1;
    }

    gnl_fss_socket_generic_destroy(generic);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_fss_socket_generic test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_fss_socket_generic struct.");
    gnl_assert(can_init_args, "can init a gnl_fss_socket_generic struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_fss_socket_generic struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_fss_socket_generic struct.");

    // the gnl_fss_socket_generic_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}