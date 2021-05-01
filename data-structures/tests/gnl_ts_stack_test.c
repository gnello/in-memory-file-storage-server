#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_ts_stack_t.c"

int can_create_a_ts_stack() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    if (stack == NULL) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_push_an_int() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    if (stack == NULL) {
        return -1;
    }

    if (gnl_ts_stack_size(stack) != 0) {
        return -1;
    }

    int expected = 1;
    gnl_ts_stack_push(stack, &expected);

    if (gnl_ts_stack_size(stack) == 0) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_pop_an_int() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    if (stack == NULL) {
        return -1;
    }

    int expected = 99;
    gnl_ts_stack_push(stack, &expected);

    if (gnl_ts_stack_size(stack) == 0) {
        return -1;
    }

    void *res = gnl_ts_stack_pop(stack);

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (expected != actual) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_push_a_string() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    if (stack == NULL) {
        return -1;
    }

    if (gnl_ts_stack_size(stack) != 0) {
        return -1;
    }

    char *expected = "this is a test.";
    gnl_ts_stack_push(stack, expected);

    if (gnl_ts_stack_size(stack) == 0) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_pop_a_string() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    if (stack == NULL) {
        return -1;
    }

    char *expected = "this is a test.";
    gnl_ts_stack_push(stack, expected);

    if (gnl_ts_stack_size(stack) == 0) {
        return -1;
    }

    void *res = gnl_ts_stack_pop(stack);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(expected, actual) != 0) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_get_the_stack_size() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    if (stack == NULL) {
        return -1;
    }

    int expected = 150;
    for (size_t i=0; i<expected; i++) {
        gnl_ts_stack_push(stack, &i);
    }

    if (gnl_ts_stack_size(stack) != expected) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_use_a_lifo_stack() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_ts_stack_push(stack, &store[i]);
    }

    int res;
    for (size_t i=1; i<=size; i++) {
        void *popped = gnl_ts_stack_pop(stack);
        if (popped == NULL) {
            return -1;
        }

        res = *(int *)popped;

        if (res != (size - i)) {
            return -1;
        }
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int can_get_null_on_an_empty_stack_pop() {
    gnl_ts_stack_t *stack;

    stack = gnl_ts_stack_init();

    void *res = gnl_ts_stack_pop(stack);

    if (res != NULL) {
        return -1;
    }

    gnl_ts_stack_destroy(stack);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_ts_stack_t test:\n\n");

    gnl_assert(can_create_a_ts_stack, "can create a thread-safe stack.");

    gnl_assert(can_push_an_int, "can push an int element into a thread-safe stack.");
    gnl_assert(can_pop_an_int, "can pop an int element from a thread-safe stack.");

    gnl_assert(can_push_a_string, "can push a string element into a thread-safe stack.");
    gnl_assert(can_pop_a_string, "can pop a string element from a thread-safe stack.");

    gnl_assert(can_get_the_stack_size, "can get the size of a thread-safe stack.");
    gnl_assert(can_use_a_lifo_stack, "can respect the LIFO protocol.");

    gnl_assert(can_get_null_on_an_empty_stack_pop, "can get null on empty thread-safe stack pop.");

    // the gnl_ts_stack_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}