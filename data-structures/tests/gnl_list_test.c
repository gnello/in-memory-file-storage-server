#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_list_t.c"
#include "./data_provider.c"

struct test {
    char *a;
    int b;
};

int test_cmp(const void *actual, const void *expected) {
    struct test actual_struct;
    struct test expected_struct;

    actual_struct = *(struct test *)actual;
    expected_struct = *(struct test *)expected;

    if (strcmp(actual_struct.a, expected_struct.a) != 0) {
        return -1;
    }

    if (actual_struct.b != expected_struct.b) {
        return -1;
    }

    return 0;
}

int can_insert_int() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, &test_int_el1);
    gnl_list_insert(&list, &test_int_el2);
    gnl_list_insert(&list, &test_int_el3);
    gnl_list_insert(&list, &test_int_el4);
    gnl_list_insert(&list, &test_int_el5);

    int res = 0;
    if (*((int *)(list->el)) != test_int_el5) {
        res = -1;
    }

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_append_int() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, &test_int_el1);
    gnl_list_insert(&list, &test_int_el2);
    gnl_list_insert(&list, &test_int_el3);
    gnl_list_insert(&list, &test_int_el4);

    gnl_list_append(&list, &test_int_el5);

    gnl_list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }

    int res = 0;
    if (*((int *)(current->el)) != test_int_el5) {
        res = -1;
    }

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_find_an_int() {
    gnl_list_t *list = NULL;

    if (gnl_list_search(list, &test_int_el2, NULL) != 0) {
        return -1;
    }

    gnl_list_insert(&list, &test_int_el1);
    gnl_list_insert(&list, &test_int_el2);
    gnl_list_insert(&list, &test_int_el3);

    int res = !gnl_list_search(list, &test_int_el2, NULL);

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_delete_an_int() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, &test_int_el1);
    gnl_list_insert(&list, &test_int_el2);
    gnl_list_insert(&list, &test_int_el3);
    gnl_list_insert(&list, &test_int_el4);
    gnl_list_insert(&list, &test_int_el5);

    if (gnl_list_search(list, &test_int_el4, NULL) != 1) {
        return -1;
    }

    gnl_list_delete(&list, &test_int_el4);

    int res = gnl_list_search(list, &test_int_el4, NULL);

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_insert_string() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, test_string_el1);
    gnl_list_insert(&list, test_string_el2);
    gnl_list_insert(&list, test_string_el3);
    gnl_list_insert(&list, test_string_el4);
    gnl_list_insert(&list, test_string_el5);

    int res = 0;
    if (strcmp((char *)(list->el), test_string_el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_append_string() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, test_string_el1);
    gnl_list_insert(&list, test_string_el2);
    gnl_list_insert(&list, test_string_el3);
    gnl_list_insert(&list, test_string_el4);

    gnl_list_append(&list, test_string_el5);

    gnl_list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }

    int res = 0;
    if (strcmp((char *)(current->el), test_string_el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_find_a_string() {
    gnl_list_t *list = NULL;

    if (gnl_list_search(list, test_string_el2, NULL) != 0) {
        return -1;
    }

    gnl_list_insert(&list, test_string_el1);
    gnl_list_insert(&list, test_string_el2);
    gnl_list_insert(&list, test_string_el3);

    int res = !gnl_list_search(list, test_string_el2, NULL);

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_delete_a_string() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, test_string_el1);
    gnl_list_insert(&list, test_string_el2);
    gnl_list_insert(&list, test_string_el3);
    gnl_list_insert(&list, test_string_el4);
    gnl_list_insert(&list, test_string_el5);

    if (gnl_list_search(list, test_string_el4, NULL) != 1) {
        return -1;
    }

    gnl_list_delete(&list, test_string_el4);

    int res = gnl_list_search(list, test_string_el4, NULL);

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_insert_struct() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, &test_struct_el1);
    gnl_list_insert(&list, &test_struct_el2);
    gnl_list_insert(&list, &test_struct_el3);
    gnl_list_insert(&list, &test_struct_el4);
    gnl_list_insert(&list, &test_struct_el5);

    int res = 0;
    if (test_cmp(list->el, (void *)&test_struct_el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_append_struct() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, &test_struct_el1);
    gnl_list_insert(&list, &test_struct_el2);
    gnl_list_insert(&list, &test_struct_el3);
    gnl_list_insert(&list, &test_struct_el4);

    gnl_list_append(&list, &test_struct_el5);

    gnl_list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }

    int res = 0;
    if (test_cmp(current->el, (void *)&test_struct_el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_find_struct() {
    gnl_list_t *list = NULL;

    if (gnl_list_search((void *)list, (void *)&test_struct_el2, test_cmp) != 0) {
        return -1;
    }

    gnl_list_insert(&list, &test_struct_el1);
    gnl_list_insert(&list, &test_struct_el2);
    gnl_list_insert(&list, &test_struct_el3);

    int res = !gnl_list_search((void *)list, (void *)&test_struct_el2, test_cmp);

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_delete_a_struct() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, &test_struct_el1);
    gnl_list_insert(&list, &test_struct_el2);
    gnl_list_insert(&list, &test_struct_el3);
    gnl_list_insert(&list, &test_struct_el4);
    gnl_list_insert(&list, &test_struct_el5);

    if (gnl_list_search((void *)list, (void *)&test_struct_el4, test_cmp) != 1) {
        return -1;
    }

    gnl_list_delete(&list, &test_struct_el4);

    int res = gnl_list_search((void *)list, (void *)&test_struct_el4, test_cmp);

    gnl_list_destroy(&list, NULL);

    return res;
}

int can_destroy_list_complex_struct() {
    gnl_list_t *list = NULL;

    gnl_list_insert(&list, (void *)test_complex_struct_init());
    gnl_list_insert(&list, (void *)test_complex_struct_init());
    gnl_list_insert(&list, (void *)test_complex_struct_init());
    gnl_list_insert(&list, (void *)test_complex_struct_init());
    gnl_list_insert(&list, (void *)test_complex_struct_init());

    gnl_list_destroy(&list, free);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_list_t test:\n\n");

    gnl_assert(can_insert_int, "can insert an int element at the beginning of the list.");
    gnl_assert(can_append_int, "can append an int element at the end of the list.");
    gnl_assert(can_find_an_int, "can check whether an int element is present into the list.");
    gnl_assert(can_delete_an_int, "can delete an int element from the list.");

    gnl_assert(can_insert_string, "can insert a string element at the beginning of the list.");
    gnl_assert(can_append_string, "can append a string element at the end of the list.");
    gnl_assert(can_find_a_string, "can check whether a string element is present into the list.");
    gnl_assert(can_delete_a_string, "can delete a string element from the list.");

    gnl_assert(can_insert_struct, "can insert a struct element at the beginning of the list.");
    gnl_assert(can_append_struct, "can append a struct element at the end of the list.");
    gnl_assert(can_find_struct, "can check whether a struct element is present into the list.");
    gnl_assert(can_delete_a_struct, "can delete a struct element from the list.");

    gnl_assert(can_destroy_list_complex_struct, "can destroy a complex struct elements list.");

    // the gnl_list_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}