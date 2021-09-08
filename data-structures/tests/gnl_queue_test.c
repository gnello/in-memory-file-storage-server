#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_queue_t.c"
#include "./data_provider.c"

int can_create_a_queue() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_enqueue_an_int() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 0) {
        return -1;
    }

    gnl_queue_enqueue(queue, &test_int_el1);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_dequeue_an_int() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    gnl_queue_enqueue(queue, &test_int_el1);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (actual != test_int_el1) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_enqueue_a_string() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 0) {
        return -1;
    }

    gnl_queue_enqueue(queue, test_string_el1);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_dequeue_a_string() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    gnl_queue_enqueue(queue, test_string_el1);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(actual, test_string_el1) != 0) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_enqueue_a_struct() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    if (gnl_queue_size(queue) != 0) {
        return -1;
    }

    gnl_queue_enqueue(queue, (void *)&test_struct_el1);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_dequeue_a_struct() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    gnl_queue_enqueue(queue, (void *)&test_struct_el1);

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    void *res = gnl_queue_dequeue(queue);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (test_struct_cmp(actual, (void *)&test_struct_el1) != 0) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_destroy_queue_complex_struct() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    gnl_queue_enqueue(queue, (void *)test_complex_struct_init());
    gnl_queue_enqueue(queue, (void *)test_complex_struct_init());
    gnl_queue_enqueue(queue, (void *)test_complex_struct_init());
    gnl_queue_enqueue(queue, (void *)test_complex_struct_init());
    gnl_queue_enqueue(queue, (void *)test_complex_struct_init());

    if (gnl_queue_size(queue) == 0) {
        return -1;
    }

    gnl_queue_destroy(queue, free);

    return 0;
}

int can_get_the_queue_size() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    if (queue == NULL) {
        return -1;
    }

    int expected = 150;
    for (size_t i=0; i<expected; i++) {
        gnl_queue_enqueue(queue, &i);
    }

    if (gnl_queue_size(queue) != expected) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_use_a_fifo_queue() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_queue_enqueue(queue, &store[i]);
    }

    int res;
    for (size_t i=0; i<size; i++) {
        void *dequeued = gnl_queue_dequeue(queue);
        if (dequeued == NULL) {
            return -1;
        }

        res = *(int *)dequeued;

        if (res != i) {
            return -1;
        }
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_get_null_on_an_empty_queue_dequeue() {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();

    void *res = gnl_queue_dequeue(queue);

    if (res != NULL) {
        return -1;
    }

    gnl_queue_destroy(queue, NULL);

    return 0;
}

int can_pass_null_queue() {
    gnl_queue_enqueue(NULL, NULL);
    gnl_queue_dequeue(NULL);
    gnl_queue_size(NULL);
    gnl_queue_destroy(NULL, NULL);

    return 0;
}

int main() {
    gnl_printf_yellow("> struct gnl_queue_t test:\n\n");

    gnl_assert(can_create_a_queue, "can create a queue.");

    gnl_assert(can_enqueue_an_int, "can push an int element into a queue.");
    gnl_assert(can_dequeue_an_int, "can pop an int element from a queue.");

    gnl_assert(can_enqueue_a_string, "can push a string element into a queue.");
    gnl_assert(can_dequeue_a_string, "can pop a string element from a queue.");

    gnl_assert(can_enqueue_a_struct, "can push a struct element into a queue.");
    gnl_assert(can_dequeue_a_struct, "can pop a struct element from a queue.");

    gnl_assert(can_destroy_queue_complex_struct, "can destroy a complex struct elements queue.");

    gnl_assert(can_get_the_queue_size, "can get the size of a queue.");
    gnl_assert(can_use_a_fifo_queue, "can respect the FIFO protocol.");

    gnl_assert(can_get_null_on_an_empty_queue_dequeue, "can get null on empty queue pop.");

    gnl_assert(can_pass_null_queue, "can give a null queue safely to the queue interface.");

    // the gnl_queue_destroy method is implicitly tested in every assertion

    printf("\n");
}