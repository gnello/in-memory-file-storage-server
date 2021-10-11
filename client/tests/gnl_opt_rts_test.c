#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_queue_t.h>
#include "../src/gnl_opt_rts.c"

int count = 0;

/**
 * Increase a counter on each invocation.
 *
 * @param string    A string.
 *
 * @return          Returns always 0.
 */
static int count_elements(const char *string) {
    count++;

    return 0;
}

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

int can_set_wait_milliseconds() {
    if (wait_milliseconds_value != 0) {
        return -1;
    }

    set_wait_milliseconds(10);

    if (wait_milliseconds_value != 10) {
        return -1;
    }

    return 0;
}

int can_wait_milliseconds() {
    time_t start = time(NULL);

    set_wait_milliseconds(3500);

    wait_milliseconds();

    time_t end = time(NULL);

    if (end - start < 3) {
        return -1;
    }

    return 0;
}

int can_enable_output() {
    if (output != 0) {
        return -1;
    }

    enable_output();

    if (output != 1) {
        return -1;
    }

    output = 0;

    return 0;
}

int can_filename_arg_walk() {
    if (count != 0) {
        return -1;
    }

    filename_arg_walk("testdir/testfile1.txt,"
                      "testdir/testfile2.txt,"
                      "testdir/testdir1/testfile3.txt,"
                      "testdir/testdir2/testfile4.txt,"
                      "testdir/testdir2/testfile5.txt,"
                      "testdir/testdir2/testfile6.txt,", count_elements);

    if (count != 6) {
        return -1;
    }

    return 0;
}

int can_not_print_command() {
    print_command('t', "test");

    char actual[100];

    int res = fread(actual, 10, 1, stdout);

    if (res > 0) {
        return -1;
    }

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_opt_rts test:\n\n");

    gnl_assert(can_scan_dir, "can scan all files in a directory and in its sub directories.");
    gnl_assert(can_scan_dir_n, "can scan only n files in a directory and in its sub directories.");
    gnl_assert(can_not_scan_dir_neg, "can not scan a negative number of files in a directory and in its sub directories.");
    gnl_assert(can_set_wait_milliseconds, "can set a time to wait between requests to the server.");
    gnl_assert(can_wait_milliseconds, "can wait between requests to the server.");
    gnl_assert(can_enable_output, "can enable output prints.");
    gnl_assert(can_filename_arg_walk, "can apply a callback to a list of elements.");
    gnl_assert(can_not_print_command, "can not print a command if enable output was not invoked.");

    printf("\n");
}