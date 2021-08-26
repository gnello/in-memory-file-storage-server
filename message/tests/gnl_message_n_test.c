#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_message_n.c"

int can_init() {
    struct gnl_message_n *message_n = gnl_message_n_init();

    if (message_n == NULL) {
        return -1;
    }

    if (message_n->number > 0) {
        return -1;
    }

    gnl_message_n_destroy(message_n);

    return 0;
}

int can_init_args() {
    struct gnl_message_n *message_n = gnl_message_n_init_with_args(15);

    if (message_n->number != 15) {
        return -1;
    }

    gnl_message_n_destroy(message_n);

    return 0;
}

int can_to_string_message() {
    struct gnl_message_n *message_n = gnl_message_n_init_with_args(15);

    char *message;
    char *expected = "0000000015";

    int res = gnl_message_n_to_string(message_n, &message);

    if (res != (strlen(expected) + 1)) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_message_n_destroy(message_n);

    return 0;
}

int can_from_string_message() {
    struct gnl_message_n *message_n = gnl_message_n_init();
    char *message = "0000000015";

    int res = gnl_message_n_from_string(message, message_n);

    if (res != 0) {
        return -1;
    }

    if (message_n->number != 15) {
        return -1;
    }

    gnl_message_n_destroy(message_n);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_message_n test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_message_n struct.");
    gnl_assert(can_init_args, "can init a gnl_message_n struct with the given arguments.");

    gnl_assert(can_to_string_message, "can get a message to string from a gnl_message_n struct.");
    gnl_assert(can_from_string_message, "can get a message from a string into a gnl_message_n struct.");

    // the gnl_message_n_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}