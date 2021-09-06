#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_file_to_pointer.h>
#include "../src/gnl_message_nq.c"

int can_init() {
    struct gnl_message_nq *message_nq = gnl_message_nq_init();

    if (message_nq == NULL) {
        return -1;
    }

    if (message_nq->number > 0) {
        return -1;
    }

    if (message_nq->queue == NULL) {
        return -1;
    }

    gnl_message_nq_destroy(message_nq);

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

    struct gnl_message_nq *message_nq = gnl_message_nq_init();
    if (message_nq == NULL) {
        return -1;
    }

    void *test1 = "test1";
    void *test2 = "test2";
    void *test3 = "test3";
    void *test4 = "test4";
    void *test5 = "test5";

    struct gnl_message_snb *message_snb1 = gnl_message_snb_init_with_args("test1", 5, test1);
    if (message_snb1 == NULL) {
        return -1;
    }

    struct gnl_message_snb *message_snb2 = gnl_message_snb_init_with_args("test2", 5, test2);
    if (message_snb2 == NULL) {
        return -1;
    }

    struct gnl_message_snb *message_snb_testfile = gnl_message_snb_init_with_args("./testfile.txt", size, content);
    if (message_snb_testfile == NULL) {
        return -1;
    }

    struct gnl_message_snb *message_snb3 = gnl_message_snb_init_with_args("test3", 5, test3);
    if (message_snb3 == NULL) {
        return -1;
    }

    struct gnl_message_snb *message_snb4 = gnl_message_snb_init_with_args("test4", 5, test4);
    if (message_snb4 == NULL) {
        return -1;
    }

    struct gnl_message_snb *message_snb5 = gnl_message_snb_init_with_args("test5", 5, test5);
    if (message_snb5 == NULL) {
        return -1;
    }

    gnl_message_nq_enqueue(message_nq, message_snb1);
    gnl_message_nq_enqueue(message_nq, message_snb2);
    gnl_message_nq_enqueue(message_nq, message_snb_testfile);
    gnl_message_nq_enqueue(message_nq, message_snb3);
    gnl_message_nq_enqueue(message_nq, message_snb4);
    gnl_message_nq_enqueue(message_nq, message_snb5);

    char *actual = NULL;
    res = gnl_message_nq_to_string(message_nq, &actual);
    if (res == -1) {
        return -1;
    }

    char *message1 = "00000000060000000005test10000000005";

    if (strcmp(message1, actual) != 0) {
        return -1;
    }

    if (memcmp(actual + 36, test1, 5) != 0) {
        return -1;
    }

    char *message2 = "0000000005test20000000005";

    if (strcmp(message2, actual + 41) != 0) {
        return -1;
    }

    if (memcmp(actual + 67, test2, 5) != 0) {
        return -1;
    }

    char message_testfile[35];
    sprintf(message_testfile, "0000000014./testfile.txt%0*ld", 10, size);

    if (strcmp(message_testfile, actual + 72) != 0) {
        return -1;
    }

    if (memcmp(actual + 72 + 35, content, size) != 0) {
        return -1;
    }

    char *message3 = "0000000005test30000000005";

    if (strcmp(message3, actual + 107 + size) != 0) {
        return -1;
    }

    if (memcmp(actual + 133 + size, test3, 5) != 0) {
        return -1;
    }

    char *message4 = "0000000005test40000000005";

    if (strcmp(message4, actual + 138 + size) != 0) {
        return -1;
    }

    if (memcmp(actual + 164 + size, test4, 5) != 0) {
        return -1;
    }

    char *message5 = "0000000005test50000000005";

    if (strcmp(message5, actual + 169 + size) != 0) {
        return -1;
    }

    if (memcmp(actual + 195 + size, test5, 5) != 0) {
        return -1;
    }

    free(content);
    free(actual);
    gnl_message_nq_destroy(message_nq);

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

    char *message = calloc(35 + size, sizeof(char *));
    if (message == NULL) {
        return -1;
    }

    sprintf(message, "00000000010000000014./testfile.txt%0*ld", 10, size);

    memcpy(message + 45, content, size);

    struct gnl_message_nq *message_nq = gnl_message_nq_init();
    res = gnl_message_nq_from_string(message, message_nq);
    if (res == -1) {
        return -1;
    }

    struct gnl_message_snb *file = gnl_message_nq_dequeue(message_nq);
    if (file == NULL) {
        return -1;
    }

    if (strcmp(file->string, "./testfile.txt") != 0) {
        return -1;
    }

    if (file->count != size) {
        return -1;
    }

    if (memcmp(file->bytes, content, size) != 0) {
        return -1;
    }

    free(message);
    free(content);
    gnl_message_nq_destroy(message_nq);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_message_nq test:\n\n");

    gnl_assert(can_init, "can init an empty gnl_message_nq struct.");

    gnl_assert(can_to_string_message, "can get a message string from a gnl_message_nq struct.");
    gnl_assert(can_from_string_message, "can get a message from a string into a gnl_message_nq struct.");

    // the gnl_message_nq_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}