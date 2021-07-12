#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_min_heap_t.c"
#include "./data_provider.c"

int can_create_a_min_heap() {
    struct gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    gnl_min_heap_destroy(mh, NULL);

    return 0;
}

int can_insert_int() {
    struct gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh, NULL);
        return -1;
    }

    gnl_min_heap_insert(mh, &test_int_el1, 1);

    int res = 0;

    if (mh->size == 0) {
        res = -1;
    }

    gnl_min_heap_destroy(mh, NULL);

    return res;
}

int can_extract_min() {
    struct gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh, NULL);
        return -1;
    }

    gnl_min_heap_insert(mh, &test_int_el1, 2);
    gnl_min_heap_insert(mh, &test_int_el2, 0);
    gnl_min_heap_insert(mh, &test_int_el3, 3);
    gnl_min_heap_insert(mh, &test_int_el4, 1);
    gnl_min_heap_insert(mh, &test_int_el5, 4);

    int res = 0;

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el2) {
        res = -1;
    }

    gnl_min_heap_destroy(mh, NULL);

    return res;
}

int can_decrease_key() {
    struct gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh, NULL);
        return -1;
    }

    gnl_min_heap_insert(mh, &test_int_el1, 2);
    gnl_min_heap_insert(mh, &test_int_el2, 0);
    gnl_min_heap_insert(mh, &test_int_el3, 3);
    gnl_min_heap_insert(mh, &test_int_el4, 1);
    gnl_min_heap_insert(mh, &test_int_el5, 4);

    //heap: el2 -> el4 -> el3 -> el1 -> el5

    gnl_min_heap_decrease_key(mh, 2, -1);

    int res = 0;

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el3) {
        res = -1;
    }

    gnl_min_heap_destroy(mh, NULL);

    return res;
}

int can_respect_heap_property() {
    struct gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh, NULL);
        return -1;
    }

    gnl_min_heap_insert(mh, &test_int_el1, 2);
    gnl_min_heap_insert(mh, &test_int_el2, 0);
    gnl_min_heap_insert(mh, &test_int_el3, 3);
    gnl_min_heap_insert(mh, &test_int_el4, 1);
    gnl_min_heap_insert(mh, &test_int_el5, 4);

    int res = 0;

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el2) {
        res = -1;
    }

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el4) {
        res = -1;
    }

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el1) {
        res = -1;
    }

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el3) {
        res = -1;
    }

    if (*(int *)gnl_min_heap_extract_min(mh) != test_int_el5) {
        res = -1;
    }

    gnl_min_heap_destroy(mh, NULL);

    return res;
}

int can_destroy_min_heap_complex_struct() {
    struct gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh, NULL);
        return -1;
    }

    gnl_min_heap_insert(mh, test_complex_struct_init(), 2);
    gnl_min_heap_insert(mh, test_complex_struct_init(), 0);
    gnl_min_heap_insert(mh, test_complex_struct_init(), 3);
    gnl_min_heap_insert(mh, test_complex_struct_init(), 1);
    gnl_min_heap_insert(mh, test_complex_struct_init(), 4);

    gnl_min_heap_destroy(mh, free);

    return 0;
}

int can_pass_null_min_heap() {
    gnl_min_heap_insert(NULL, NULL, 0);
    gnl_min_heap_extract_min(NULL);
    gnl_min_heap_decrease_key(NULL, 0, 0);
    gnl_min_heap_destroy(NULL, NULL);

    return 0;
}

int main() {
    gnl_printf_yellow("> struct gnl_min_heap_t tests:\n\n");

    gnl_assert(can_create_a_min_heap, "can create a min heap.");
    gnl_assert(can_insert_int, "can insert an int element into a min heap.");
    gnl_assert(can_extract_min, "can extract the min of a min heap.");
    gnl_assert(can_decrease_key, "can decrease a key of a min heap.");
    gnl_assert(can_respect_heap_property, "can respect the min heap property.");

    gnl_assert(can_destroy_min_heap_complex_struct, "can destroy a complex struct elements min heap.");

    gnl_assert(can_pass_null_min_heap, "can give a null min heap safely to the min heap interface.");

    // the gnl_min_heap_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}