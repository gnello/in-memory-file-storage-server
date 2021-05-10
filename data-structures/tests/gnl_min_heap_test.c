#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_min_heap_t.c"
#include "./data_provider.c"

int can_create_a_min_heap() {
    gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    gnl_min_heap_destroy(mh);

    return 0;
}

int can_insert_int() {
    gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh);
        return -1;
    }

    gnl_min_heap_insert(mh, &test_int_el1, 1);

    int res = 0;

    if (mh->size == 0) {
        res = -1;
    }

    gnl_min_heap_destroy(mh);

    return res;
}

int can_extract_min() {
    gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh);
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

    gnl_min_heap_destroy(mh);

    return res;
}

int can_decrease_key() {
    gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh);
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

    gnl_min_heap_destroy(mh);

    return res;
}

int can_respect_heap_property() {
    gnl_min_heap_t *mh;

    mh = gnl_min_heap_init();

    if (mh == NULL) {
        return -1;
    }

    if (mh->size != 0) {
        gnl_min_heap_destroy(mh);
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

    gnl_min_heap_destroy(mh);

    return res;
}

int main() {
    gnl_printf_yellow("> gnl_min_heap_t tests:\n\n");

    gnl_assert(can_create_a_min_heap, "can create a min heap.");
    gnl_assert(can_insert_int, "can insert an int element into a min heap.");
    gnl_assert(can_extract_min, "can extract the min of a min heap.");
    gnl_assert(can_decrease_key, "can decrease a key of a min heap.");
    gnl_assert(can_respect_heap_property, "can respect the min heap property.");

    // the gnl_min_heap_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}