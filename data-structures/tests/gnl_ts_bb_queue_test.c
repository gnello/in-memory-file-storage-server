#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_ts_bb_queue_t.c"
#include "./data_provider.c"

int can_create_a_ts_queue() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(100);

    if (queue == NULL) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_enqueue_an_int() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(100);

    if (queue == NULL) {
        return -1;
    }

    if (gnl_ts_bb_queue_size(queue) != 0) {
        return -1;
    }

    gnl_ts_bb_queue_enqueue(queue, &test_int_el1);

    if (gnl_ts_bb_queue_size(queue) == 0) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_dequeue_an_int() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(100);

    if (queue == NULL) {
        return -1;
    }

    gnl_ts_bb_queue_enqueue(queue, &test_int_el1);

    if (gnl_ts_bb_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_ts_bb_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (test_int_el1 != actual) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_enqueue_a_string() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(100);

    if (queue == NULL) {
        return -1;
    }

    if (gnl_ts_bb_queue_size(queue) != 0) {
        return -1;
    }

    gnl_ts_bb_queue_enqueue(queue, test_string_el1);

    if (gnl_ts_bb_queue_size(queue) == 0) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_dequeue_a_string() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(100);

    if (queue == NULL) {
        return -1;
    }

    gnl_ts_bb_queue_enqueue(queue, test_string_el1);

    if (gnl_ts_bb_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_ts_bb_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(test_string_el1, actual) != 0) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_destroy_queue_complex_struct() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(100);

    if (queue == NULL) {
        return -1;
    }

    gnl_ts_bb_queue_enqueue(queue, test_complex_struct_init());
    gnl_ts_bb_queue_enqueue(queue, test_complex_struct_init());
    gnl_ts_bb_queue_enqueue(queue, test_complex_struct_init());
    gnl_ts_bb_queue_enqueue(queue, test_complex_struct_init());
    gnl_ts_bb_queue_enqueue(queue, test_complex_struct_init());

    if (gnl_ts_bb_queue_size(queue) == 0) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, free);

    return 0;
}

int can_get_the_queue_size() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(200);

    if (queue == NULL) {
        return -1;
    }

    int expected = 150;
    for (size_t i=0; i<expected; i++) {
        gnl_ts_bb_queue_enqueue(queue, &i);
    }

    if (gnl_ts_bb_queue_size(queue) != expected) {
        return -1;
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_use_a_fifo_queue() {
    struct gnl_ts_bb_queue_t *queue;

    queue = gnl_ts_bb_queue_init(200);

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_ts_bb_queue_enqueue(queue, &store[i]);
    }

    int res;
    for (size_t i=0; i<size; i++) {
        void *dequeued = gnl_ts_bb_queue_dequeue(queue);
        if (dequeued == NULL) {
            return -1;
        }

        res = *(int *)dequeued;

        if (res != i) {
            return -1;
        }
    }

    gnl_ts_bb_queue_destroy(queue, NULL);

    return 0;
}

int can_pass_null_queue() {
    gnl_ts_bb_queue_enqueue(NULL, NULL);
    gnl_ts_bb_queue_dequeue(NULL);
    gnl_ts_bb_queue_size(NULL);
    gnl_ts_bb_queue_destroy(NULL, NULL);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_ts_bb_queue_t test:\n\n");

    gnl_assert(can_create_a_ts_queue, "can create a thread-safe blocking bounded queue.");

    gnl_assert(can_enqueue_an_int, "can push an int element into a thread-safe blocking bounded queue.");
    gnl_assert(can_dequeue_an_int, "can pop an int element from a thread-safe blocking bounded queue.");

    gnl_assert(can_enqueue_a_string, "can push a string element into a thread-safe blocking bounded queue.");
    gnl_assert(can_dequeue_a_string, "can pop a string element from a thread-safe blocking bounded queue.");

    gnl_assert(can_destroy_queue_complex_struct, "can destroy a thread-safe blocking bounded queue containing complex struct elements.");

    gnl_assert(can_get_the_queue_size, "can get the size of a thread-safe blocking bounded queue.");
    gnl_assert(can_use_a_fifo_queue, "can respect the FIFO protocol.");

    gnl_assert(can_pass_null_queue, "can give a null queue safely to the thread-safe blocking bounded queue interface.");

    // the gnl_ts_bb_queue_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}