#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_socket_request_sb.c"

int can_init() {
    struct gnl_socket_request_sb *request_sb = gnl_socket_request_sb_init();

    if (request_sb == NULL) {
        return -1;
    }

    if (request_sb->string != NULL) {
        return -1;
    }

    if (request_sb->bytes != NULL) {
        return -1;
    }

    gnl_socket_request_sb_destroy(request_sb);

    return 0;
}

int can_init_args() {
    struct gnl_socket_request_sb *request_sb = gnl_socket_request_sb_init_with_args("/fake/path", "\x41\x42\x43\x44");

    if (strcmp(request_sb->string, "/fake/path") != 0) {
        return -1;
    }

    if (strcmp(request_sb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_socket_request_sb_destroy(request_sb);

    return 0;
}

int can_build_message() {
    struct gnl_socket_request_sb *request_sb = gnl_socket_request_sb_init_with_args("/fake/path", "\x41\x42\x43\x44");

    char *message;
    char *expected = "0000000010/fake/path0000000004ABCD";

    int res = gnl_socket_request_sb_write(*request_sb, &message);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    free(message);
    gnl_socket_request_sb_destroy(request_sb);

    return 0;
}

int can_read_message() {
    struct gnl_socket_request_sb *request_sb = gnl_socket_request_sb_init();
    char *message = "0000000010/fake/path0000000004ABCD";

    int res = gnl_socket_request_sb_read(message, request_sb);

    if (res != 0) {
        return -1;
    }

    if (strcmp(request_sb->string, "/fake/path") != 0) {
        return -1;
    }

    if (strcmp(request_sb->bytes, "ABCD") != 0) {
        return -1;
    }

    gnl_socket_request_sb_destroy(request_sb);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_socket_request_sb test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_socket_request_sb struct.");
    gnl_assert(can_init_args, "can init a gnl_socket_request_sb struct with the given arguments.");

    gnl_assert(can_build_message, "can build a message from a gnl_socket_request_sb struct.");
    gnl_assert(can_read_message, "can read a message into a gnl_socket_request_sb struct.");

    // the gnl_socket_request_sb_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}