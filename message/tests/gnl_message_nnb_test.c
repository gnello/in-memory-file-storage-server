#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_message_nnb.c"

int can_init() {
    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init();

    if (message_nnb == NULL) {
        return -1;
    }

    if (message_nnb->number > 0) {
        return -1;
    }

    if (message_nnb->count > 0) {
        return -1;
    }

    if (message_nnb->bytes != NULL) {
        return -1;
    }

    gnl_message_nnb_destroy(message_nnb);

    return 0;
}

int can_init_args() {
    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init_with_args(220510, 4,"\x41\x42\x43\x44");

    if (message_nnb->number != 220510) {
        return -1;
    }

    if (message_nnb->count != 4) {
        return -1;
    }

    if (strcmp(message_nnb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_message_nnb_destroy(message_nnb);

    return 0;
}

int can_build_message() {
    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init_with_args(220510, 4, "\x41\x42\x43\x44");

    char *message;
    char *expected = "00002205100000000004ABCD";

    int res = gnl_message_nnb_write(*message_nnb, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_message_nnb_destroy(message_nnb);

    return 0;
}

int can_read_message() {
    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init();
    char *message = "00002205100000000004ABCD";

    int res = gnl_message_nnb_read(message, message_nnb);

    if (res != 0) {
        return -1;
    }

    if (message_nnb->number != 220510) {
        return -1;
    }

    if (message_nnb->count != 4) { printf("count: %ld\n", message_nnb->count);
        return -1;
    }

    if (strcmp(message_nnb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_message_nnb_destroy(message_nnb);

    return 0;
}

int main() {
    gnl_printf_yellow("> message_nnb test:\n\n");

    gnl_assert(can_init, "can init an empty message_nnb struct.");
    gnl_assert(can_init_args, "can init a message_nnb struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a message_nnb struct.");
    gnl_assert(can_read_message, "can read a message into a message_nnb struct.");

    // the message_nnb_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}