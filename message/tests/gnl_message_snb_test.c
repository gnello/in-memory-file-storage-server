#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_file_to_pointer.h>
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
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_message_snb *message_snb = gnl_message_snb_init_with_args("/fake/path", size, content);

    if (strcmp(message_snb->string, "/fake/path") != 0) {
        return -1;
    }

    if (message_snb->count != size) {
        return -1;
    }

    if (memcmp(message_snb->bytes, content, size) != 0) {
        return -1;
    }

    free(content);
    gnl_message_snb_destroy(message_snb);

    return 0;
}

int can_to_string_message() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_message_snb *message_snb = gnl_message_snb_init_with_args("/fake/path", size, content);

    char *expected = calloc(31, sizeof(char *));
    if (expected == NULL) {
        return -1;
    }

    sprintf(expected, "0000000010/fake/path%0*ld", 10, size);

    char *message;

    res = gnl_message_snb_to_string(message_snb, &message);

    if (res != (strlen(expected) + 1) + size) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    if (memcmp(message + 31, content, size) != 0) {
        return -1;
    }

    free(content);
    free(message);
    free(expected);
    gnl_message_snb_destroy(message_snb);

    return 0;
}

int can_from_string_message() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_message_snb *message_snb = gnl_message_snb_init();

    char *message = calloc(31 + size, sizeof(char *));
    if (message == NULL) {
        return -1;
    }

    sprintf(message, "0000000010/fake/path%0*ld", 10, size);

    memcpy(message + 31, content, size);

    res = gnl_message_snb_from_string(message, message_snb);

    if (res != 0) {
        return -1;
    }

    if (strcmp(message_snb->string, "/fake/path") != 0) {
        return -1;
    }

    if (message_snb->count != size) {
        return -1;
    }

    if (memcmp(message_snb->bytes, content, size) != 0) {
        return -1;
    }

    free(content);
    free(message);
    gnl_message_snb_destroy(message_snb);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_message_snb test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_message_snb struct.");
    gnl_assert(can_init_args, "can init a gnl_message_snb struct with the given arguments.");

    gnl_assert(can_to_string_message, "can get a message string from a gnl_message_snb struct.");
    gnl_assert(can_from_string_message, "can get a message from a string into a gnl_message_snb struct.");

    // the gnl_message_snb_destroy method is implicitly tested in every assertion

    printf("\n");
}