#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_message_s.c"

int can_init() {
    struct gnl_socket_message_s *message_s = gnl_socket_message_s_init();

    if (message_s == NULL) {
        return -1;
    }

    if (message_s->string != NULL) {
        return -1;
    }

    gnl_socket_message_s_destroy(message_s);

    return 0;
}

int can_init_args() {
    struct gnl_socket_message_s *message_s = gnl_socket_message_s_init_with_args("/fake/path");

    if (strcmp(message_s->string, "/fake/path") != 0) {
        return -1;
    }

    gnl_socket_message_s_destroy(message_s);

    return 0;
}

int can_build_message() {
    struct gnl_socket_message_s *message_s = gnl_socket_message_s_init_with_args("/fake/path");

    char *message;
    char *expected = "0000000010/fake/path";

    int res = gnl_socket_message_s_write(*message_s, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_socket_message_s_destroy(message_s);

    return 0;
}

int can_read_message() {
    struct gnl_socket_message_s *message_s = gnl_socket_message_s_init();
    char *message = "0000000010/fake/path";

    int res = gnl_socket_message_s_read(message, message_s);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message_s->string, "/fake/path") != 0) {
        return -1;
    }

    gnl_socket_message_s_destroy(message_s);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_message_s test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_socket_message_s struct.");
    gnl_assert(can_init_args, "can init a gnl_socket_message_s struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_socket_message_s struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_socket_message_s struct.");

    // the gnl_socket_message_s_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}