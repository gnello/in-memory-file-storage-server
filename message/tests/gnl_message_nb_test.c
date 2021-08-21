#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_message_nb.c"

int can_init() {
    struct gnl_message_nb *message_nb = gnl_message_nb_init();

    if (message_nb == NULL) {
        return -1;
    }

    if (message_nb->number > 0) {
        return -1;
    }

    if (message_nb->bytes != NULL) {
        return -1;
    }

    gnl_message_nb_destroy(message_nb);

    return 0;
}

int can_init_args() {
    struct gnl_message_nb *message_nb = gnl_message_nb_init_with_args(220510, "\x41\x42\x43\x44");

    if (message_nb->number != 220510) {
        return -1;
    }

    if (strcmp(message_nb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_message_nb_destroy(message_nb);

    return 0;
}

int can_build_message() {
    struct gnl_message_nb *message_nb = gnl_message_nb_init_with_args(220510, "\x41\x42\x43\x44");

    char *message;
    char *expected = "00002205100000000004ABCD";

    int res = gnl_message_nb_write(*message_nb, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_message_nb_destroy(message_nb);

    return 0;
}

int can_read_message() {
    struct gnl_message_nb *message_nb = gnl_message_nb_init();
    char *message = "00002205100000000004ABCD";

    int res = gnl_message_nb_read(message, message_nb);

    if (res != 0) {
        return -1;
    }

    if (message_nb->number != 220510) {
        return -1;
    }

    if (strcmp(message_nb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_message_nb_destroy(message_nb);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_message_nb test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_message_nb struct.");
    gnl_assert(can_init_args, "can init a gnl_message_nb struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_message_nb struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_message_nb struct.");

    // the gnl_message_nb_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}