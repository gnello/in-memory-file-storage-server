#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_stack_t.c"
#include "./data_provider.c"

int can_create_a_stack() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_push_an_int() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    if (gnl_stack_size(stack) != 0) {
        return -1;
    }

    gnl_stack_push(stack, &test_int_el1);

    if (gnl_stack_size(stack) == 0) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_pop_an_int() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    gnl_stack_push(stack, &test_int_el1);

    if (gnl_stack_size(stack) == 0) {
        return -1;
    }

    void *res = gnl_stack_pop(stack);

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (test_int_el1 != actual) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_push_a_string() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    if (gnl_stack_size(stack) != 0) {
        return -1;
    }

    gnl_stack_push(stack, test_string_el1);

    if (gnl_stack_size(stack) == 0) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_pop_a_string() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    gnl_stack_push(stack, test_string_el1);

    if (gnl_stack_size(stack) == 0) {
        return -1;
    }

    void *res = gnl_stack_pop(stack);

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(test_string_el1, actual) != 0) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_destroy_stack_complex_struct() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    gnl_stack_push(stack, test_complex_struct_init());
    gnl_stack_push(stack, test_complex_struct_init());
    gnl_stack_push(stack, test_complex_struct_init());
    gnl_stack_push(stack, test_complex_struct_init());
    gnl_stack_push(stack, test_complex_struct_init());

    if (gnl_stack_size(stack) == 0) {
        return -1;
    }

    gnl_stack_destroy(stack, free);

    return 0;
}

int can_get_the_stack_size() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    if (stack == NULL) {
        return -1;
    }

    int expected = 150;
    for (size_t i=0; i<expected; i++) {
        gnl_stack_push(stack, &i);
    }

    if (gnl_stack_size(stack) != expected) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_use_a_lifo_stack() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    int size = 150;
    int store[size];

    for (size_t i=0; i<size; i++) {
        store[i] = i;
        gnl_stack_push(stack, &store[i]);
    }

    int res;
    for (size_t i=1; i<=size; i++) {
        void *popped = gnl_stack_pop(stack);
        if (popped == NULL) {
            return -1;
        }

        res = *(int *)popped;

        if (res != (size - i)) {
            return -1;
        }
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_get_null_on_an_empty_stack_pop() {
    struct gnl_stack_t *stack;

    stack = gnl_stack_init();

    void *res = gnl_stack_pop(stack);

    if (res != NULL) {
        return -1;
    }

    gnl_stack_destroy(stack, NULL);

    return 0;
}

int can_pass_null_stack() {
    gnl_stack_push(NULL, NULL);
    gnl_stack_pop(NULL);
    gnl_stack_size(NULL);
    gnl_stack_destroy(NULL, NULL);

    return 0;
}

int main() {
    gnl_printf_yellow("> struct gnl_stack_t tests:\n\n");

    gnl_assert(can_create_a_stack, "can create a stack.");

    gnl_assert(can_push_an_int, "can push an int element into a stack.");
    gnl_assert(can_pop_an_int, "can pop an int element from a stack.");

    gnl_assert(can_push_a_string, "can push a string element into a stack.");
    gnl_assert(can_pop_a_string, "can pop a string element from a stack.");

    gnl_assert(can_destroy_stack_complex_struct, "can destroy a complex struct elements stack.");

    gnl_assert(can_get_the_stack_size, "can get the size of a stack.");
    gnl_assert(can_use_a_lifo_stack, "can respect the LIFO protocol.");

    gnl_assert(can_get_null_on_an_empty_stack_pop, "can get null on empty stack pop.");

    gnl_assert(can_pass_null_stack, "can give a null stack safely to the stack interface.");

    // the gnl_stack_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}