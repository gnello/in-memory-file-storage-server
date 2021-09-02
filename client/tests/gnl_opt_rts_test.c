#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_queue_t.h>
#include "../src/gnl_opt_rts.c"

int can_scan_dir() {
    char *expected[6] = {
        "./testdir/testfile1.txt",
        "./testdir/testdir1/testfile3.txt",
        "./testdir/testdir2/testfile6.txt",
        "./testdir/testdir2/testfile4.txt",
        "./testdir/testdir2/testfile5.txt",
        "./testdir/testfile2.txt"
    };
    char *buffer;
    char *res_ptr;

    char *actual[6];

    struct gnl_queue_t *queue = gnl_opt_rts_scan_dir("./testdir", 0);

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 6) {
        return -1;
    }

    for (size_t i=0; i<6; i++) {
        actual[i] = gnl_queue_dequeue(queue);
    }

    // for each expected
    for (size_t i=0; i<6; i++) {

        int found = 0;

        buffer = (char *)calloc(PATH_MAX, sizeof(char));
        if (buffer == NULL) {
            return -1;
        }

        res_ptr = realpath(expected[i], buffer);
        if (res_ptr == NULL) {
            return -1;
        }

        // search in actual
        for (size_t k=0; k<6; k++) {

            if (strcmp(actual[k], buffer) == 0) {
                found = 1;
            }
        }

        free(buffer);

        if (found != 1) {
            return -1;
        }
    }

    for (size_t i=0; i<6; i++) {
        free(actual[i]);
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_scan_dir_n() {
    struct gnl_queue_t *queue = gnl_opt_rts_scan_dir("./testdir", 3);

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 3) {
        return -1;
    }

    gnl_queue_destroy(queue, free);

    return 0;
}

int can_not_scan_dir_neg() {
    struct gnl_queue_t *queue = gnl_opt_rts_scan_dir("./testdir", -3);

    if (queue != NULL) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_opt_rts test:\n\n");

    gnl_assert(can_scan_dir, "can scan all files in a directory and in its sub directories.");
    gnl_assert(can_scan_dir_n, "can scan only n files in a directory and in its sub directories.");
    gnl_assert(can_not_scan_dir_neg, "can not scan a negative number of files in a directory and in its sub directories.");

    printf("\n");
}