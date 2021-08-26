#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_message_s.c"

int can_init() {
    struct gnl_message_s *message_s = gnl_message_s_init();

    if (message_s == NULL) {
        return -1;
    }

    if (message_s->string != NULL) {
        return -1;
    }

    gnl_message_s_destroy(message_s);

    return 0;
}

int can_init_args() {
    struct gnl_message_s *message_s = gnl_message_s_init_with_args("/fake/path");

    if (strcmp(message_s->string, "/fake/path") != 0) {
        return -1;
    }

    gnl_message_s_destroy(message_s);

    return 0;
}

int can_to_string_message() {
    struct gnl_message_s *message_s = gnl_message_s_init_with_args("/fake/path");

    char *message;
    char *expected = "0000000010/fake/path";

    int res = gnl_message_s_to_string(message_s, &message);

    if (res != (strlen(expected) + 1)) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_message_s_destroy(message_s);

    return 0;
}

int can_from_string_message() {
    struct gnl_message_s *message_s = gnl_message_s_init();
    char *message = "0000000010/fake/path";

    int res = gnl_message_s_from_string(message, message_s);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message_s->string, "/fake/path") != 0) {
        return -1;
    }

    gnl_message_s_destroy(message_s);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_message_s test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_message_s struct.");
    gnl_assert(can_init_args, "can init a gnl_message_s struct with the given arguments.");

    gnl_assert(can_to_string_message, "can get a message to string from a gnl_message_s struct.");
    gnl_assert(can_from_string_message, "can get a message from a string into a gnl_message_s struct.");

    // the gnl_message_s_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}