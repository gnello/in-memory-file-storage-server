#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_file_to_pointer.h>
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
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init_with_args(220510, size,content);

    if (message_nnb->number != 220510) {
        return -1;
    }

    if (message_nnb->count != size) {
        return -1;
    }

    if (memcmp(message_nnb->bytes, content, size) != 0) {
        return -1;
    }

    free(content);
    gnl_message_nnb_destroy(message_nnb);

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

    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init_with_args(220510, size, content);

    char *expected = calloc(21 + size, sizeof(char *));
    if (expected == NULL) {
        return -1;
    }

    sprintf(expected, "0000220510%0*ld", 10, size);

    char *message;

    res = gnl_message_nnb_to_string(message_nnb, &message);

    if (res != (strlen(expected) + 1) + size) {
        return -1;
    }

    if (strcmp(message, expected) != 0) {
        return -1;
    }

    if (memcmp(message + 21, content, size) != 0) {
        return -1;
    }

    free(content);
    free(message);
    free(expected);
    gnl_message_nnb_destroy(message_nnb);

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

    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init();

    char *message = calloc(21 + size, sizeof(char *));
    if (message == NULL) {
        return -1;
    }

    sprintf(message, "0000220510%0*ld", 10, size);

    memcpy(message + 21, content, size);

    res = gnl_message_nnb_from_string(message, message_nnb);

    if (res != 0) {
        return -1;
    }

    if (message_nnb->number != 220510) {
        return -1;
    }

    if (message_nnb->count != size) {
        return -1;
    }

    if (memcmp(message_nnb->bytes, content, size) != 0) {
        return -1;
    }

    free(content);
    free(message);
    gnl_message_nnb_destroy(message_nnb);

    return 0;
}

int main() {
    gnl_printf_yellow("> message_nnb test:\n\n");

    gnl_assert(can_init, "can init an empty message_nnb struct.");
    gnl_assert(can_init_args, "can init a message_nnb struct with the given arguments.");

    gnl_assert(can_to_string_message, "can get a message string from a message_nnb struct.");
    gnl_assert(can_from_string_message, "can get a message from a string into a message_nnb struct.");

    // the message_nnb_destroy method is implicitly tested in every assertion

    printf("\n");
}