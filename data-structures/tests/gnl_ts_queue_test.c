#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_ts_queue_t.c"

int can_create_a_ts_queue() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_enqueue_an_int() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        return -1;
    }

    if (gnl_ts_queue_size(queue) != 0) {
        return -1;
    }

    int expected = 1;
    gnl_ts_queue_enqueue(queue, &expected);

    if (gnl_ts_queue_size(queue) == 0) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_dequeue_an_int() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        return -1;
    }

    int expected = 99;
    gnl_ts_queue_enqueue(queue, &expected);

    if (gnl_ts_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_ts_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (expected != actual) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_enqueue_a_string() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        return -1;
    }

    if (gnl_ts_queue_size(queue) != 0) {
        return -1;
    }

    char *expected = "this is a test.";
    gnl_ts_queue_enqueue(queue, expected);

    if (gnl_ts_queue_size(queue) == 0) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_dequeue_a_string() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        return -1;
    }

    char *expected = "this is a test.";
    gnl_ts_queue_enqueue(queue, expected);

    if (gnl_ts_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_ts_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(expected, actual) != 0) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_get_the_queue_size() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        return -1;
    }

    int expected = 150;
    for (size_t i=0; i<expected; i++) {
        gnl_ts_queue_enqueue(queue, &i);
    }

    if (gnl_ts_queue_size(queue) != expected) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_use_a_fifo_queue() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_ts_queue_enqueue(queue, &store[i]);
    }

    int res;
    for (size_t i=0; i<size; i++) {
        res = *(int *)gnl_ts_queue_dequeue(queue);

        if (res != i) {
            return -1;
        }
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int can_get_null_on_an_empty_queue_dequeue() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    void *res = gnl_ts_queue_dequeue(queue);

    if (res != NULL) {
        return -1;
    }

    gnl_ts_queue_destroy(queue);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_ts_queue_t test:\n\n");

    gnl_assert(can_create_a_ts_queue, "can create a thread-safe queue.");

    gnl_assert(can_enqueue_an_int, "can push an int element into a thread-safe queue.");
    gnl_assert(can_dequeue_an_int, "can pop an int element from a thread-safe queue.");

    gnl_assert(can_enqueue_a_string, "can push a string element into a thread-safe queue.");
    gnl_assert(can_dequeue_a_string, "can pop a string element from a thread-safe queue.");

    gnl_assert(can_get_the_queue_size, "can get the size of a thread-safe queue.");
    gnl_assert(can_use_a_fifo_queue, "can respect the FIFO protocol.");

    gnl_assert(can_get_null_on_an_empty_queue_dequeue, "can get null on empty thread-safe queue pop.");

    // the gnl_ts_queue_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}