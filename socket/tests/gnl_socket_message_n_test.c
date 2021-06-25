#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_message_n.c"

int can_init() {
    struct gnl_socket_message_n *message_n = gnl_socket_message_n_init();

    if (message_n == NULL) {
        return -1;
    }

    if (message_n->number > 0) {
        return -1;
    }

    gnl_socket_message_n_destroy(message_n);

    return 0;
}

int can_init_args() {
    struct gnl_socket_message_n *message_n = gnl_socket_message_n_init_with_args(15);

    if (message_n->number != 15) {
        return -1;
    }

    gnl_socket_message_n_destroy(message_n);

    return 0;
}

int can_build_message() {
    struct gnl_socket_message_n *message_n = gnl_socket_message_n_init_with_args(15);

    char *message;
    char *expected = "0000000015";

    int res = gnl_socket_message_n_write(*message_n, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_socket_message_n_destroy(message_n);

    return 0;
}

int can_read_message() {
    struct gnl_socket_message_n *message_n = gnl_socket_message_n_init();
    char *message = "0000000015";

    int res = gnl_socket_message_n_read(message, message_n);

    if (res != 0) {
        return -1;
    }

    if (message_n->number != 15) {
        return -1;
    }

    gnl_socket_message_n_destroy(message_n);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_message_n test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_socket_message_n struct.");
    gnl_assert(can_init_args, "can init a gnl_socket_message_n struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_socket_message_n struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_socket_message_n struct.");

    // the gnl_socket_message_n_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}