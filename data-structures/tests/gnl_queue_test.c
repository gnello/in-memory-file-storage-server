#include <stdio.h>
#include <string.h>
#include "../../helpers/includes/gnl_colorshell.h"
#include "../../helpers/includes/gnl_assert.h"
#include "../includes/gnl_queue_t.h"

int can_create_a_queue() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    if (queue == NULL) {
        return -1;
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_enqueue_an_int() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 0) {
        return -1;
    }

    int expected = 1;
    gnl_queue_push(&queue, &expected);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_dequeue_an_int() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    if (queue == NULL) {
        return -1;
    }

    int expected = 99;
    gnl_queue_push(&queue, &expected);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_queue_pop(&queue);

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (expected != actual) {
        return -1;
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_enqueue_a_string() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 0) {
        return -1;
    }

    char *expected = "this is a test.";
    gnl_queue_push(&queue, expected);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_dequeue_a_string() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    if (queue == NULL) {
        return -1;
    }

    char *expected = "this is a test.";
    gnl_queue_push(&queue, expected);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_queue_pop(&queue);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(expected, actual) != 0) {
        return -1;
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_get_the_queue_size() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    if (queue == NULL) {
        return -1;
    }

    int expected = 150;
    for (size_t i=0; i<expected; i++) {
        gnl_queue_push(&queue, &i);
    }

    if (gnl_queue_size(queue) != expected) {
        return -1;
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_use_a_fifo_queue() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_FIFO_QUEUE);

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_queue_push(&queue, &store[i]);
    }

    int res;
    for (size_t i=0; i<size; i++) {
        res = *(int *)gnl_queue_pop(&queue);

        if (res != i) {
            return -1;
        }
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int can_use_a_lifo_queue() {
    gnl_queue_t *queue;

    queue = gnl_queue_init(GNL_LIFO_QUEUE);

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_queue_push(&queue, &store[i]);
    }

    int res;
    for (size_t i=1; i<=size; i++) {
        res = *(int *)gnl_queue_pop(&queue);

        if (res != (size - i)) {
            return -1;
        }
    }

    gnl_queue_destroy(&queue);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_queue tests:\n\n");

    gnl_assert(can_create_a_queue, "can create a queue.");

    gnl_assert(can_enqueue_an_int, "can push an int element into a queue.");
    gnl_assert(can_dequeue_an_int, "can pop an int element from a queue.");

    gnl_assert(can_enqueue_a_string, "can push a string element into a queue.");
    gnl_assert(can_dequeue_a_string, "can pop a string element from a queue.");

    gnl_assert(can_get_the_queue_size, "can get the size of a queue.");
    gnl_assert(can_use_a_fifo_queue, "can use a FIFO queue.");
    gnl_assert(can_use_a_lifo_queue, "can use a LIFO queue.");

    // the gnl_queue_destroy method is implicit tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}