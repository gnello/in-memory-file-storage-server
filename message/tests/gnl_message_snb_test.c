#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_message_snb.c"

int can_init() {
    struct gnl_message_snb *message_snb = gnl_message_snb_init();

    if (message_snb == NULL) {
        return -1;
    }

    if (message_snb->string != NULL) {
        return -1;
    }

    if (message_snb->count > 0) {
        return -1;
    }

    if (message_snb->bytes != NULL) {
        return -1;
    }

    gnl_message_snb_destroy(message_snb);

    return 0;
}

int can_init_args() {
    struct gnl_message_snb *message_snb = gnl_message_snb_init_with_args("/fake/path", 4, "\x41\x42\x43\x44");

    if (strcmp(message_snb->string, "/fake/path") != 0) {
        return -1;
    }

    if (message_snb->count != 4) {
        return -1;
    }

    if (strcmp(message_snb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_message_snb_destroy(message_snb);

    return 0;
}

int can_to_string_message() {
    struct gnl_message_snb *message_snb = gnl_message_snb_init_with_args("/fake/path", 4, "\x41\x42\x43\x44");

    char *message;
    char *expected = "0000000010/fake/path0000000004";
    int bytes = 4; // x41 x42 x43 x44 = 4 bytes

    int res = gnl_message_snb_to_string(message_snb, &message);

    if (res != (strlen(expected) + 1) + bytes) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_message_snb_destroy(message_snb);

    return 0;
}

int can_from_string_message() {
    struct gnl_message_snb *message_snb = gnl_message_snb_init();
    char *message = "0000000010/fake/path0000000004ABCD";

    int res = gnl_message_snb_from_string(message, message_snb);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message_snb->string, "/fake/path") != 0) {
        return -1;
    }

    if (message_snb->count != 4) {
        return -1;
    }

    if (strcmp(message_snb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_message_snb_destroy(message_snb);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_message_snb test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_message_snb struct.");
    gnl_assert(can_init_args, "can init a gnl_message_snb struct with the given arguments.");

    gnl_assert(can_to_string_message, "can get a message string from a gnl_message_snb struct.");
    gnl_assert(can_from_string_message, "can get a message from a string into a gnl_message_snb struct.");

    // the gnl_message_snb_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}