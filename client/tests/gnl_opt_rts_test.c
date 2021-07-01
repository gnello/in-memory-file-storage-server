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

    struct gnl_queue_t *queue = gnl_opt_rts_scan_dir("./testdir", 0);

    if (queue == NULL) {
        return -1;
    }

    for (size_t i=0; i<6; i++) {
        char *filename = gnl_queue_dequeue(queue);

        if (strcmp(filename, expected[i]) != 0) {
            free(filename);
            return -1;
        }

        free(filename);
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