#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_ternary_search_tree_t.c"
#include "./data_provider.c"

int can_put_an_int() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", &test_int_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (test_int_el1 != actual) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove_an_int() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", &test_int_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    int actual = *(int *)res;

    if (test_int_el1 != actual) {
        return -1;
    }
    
    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "test", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res != NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_put_a_string() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", test_string_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(test_string_el1, actual) != 0) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove_a_string() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", test_string_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(test_string_el1, actual) != 0) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "test", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res != NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_put_a_struct() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "struct_1", (void *)&test_struct_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "struct_1");

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (test_struct_cmp(actual, (void *)&test_struct_el1) != 0) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove_a_struct() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "struct_1", (void *)&test_struct_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "struct_1");

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (test_struct_cmp(actual, (void *)&test_struct_el1) != 0) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "struct_1", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "struct_1");
    if (res != NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_not_put_empty_key() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    int res = gnl_ternary_search_tree_put(&ternary_search_tree, "", &test_int_el1);

    if (res != -1) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_update_an_element() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", test_string_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp(test_string_el1, actual) != 0) {
        return -1;
    }

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", test_string_el2);

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    actual = (char *)res;

    if (strcmp(test_string_el2, actual) != 0) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_destroy_ternary_search_tree_complex_struct() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;
    char *keys[5] = {"a", "b", "c", "d", "e"};

    for (size_t i=0; i<5; i++) {
        gnl_ternary_search_tree_put(&ternary_search_tree, keys[i], test_complex_struct_init());
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, free);

    return 0;
}

int can_remove_an_allocated_pointer() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    char *test = malloc(5 * sizeof (char));
    strcpy(test, "ciao");

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", test);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res == NULL) {
        return -1;
    }

    char *actual = (char *)res;

    if (strcmp("ciao", actual) != 0) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "test", free);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res != NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, free);

    return 0;
}

int can_get_null_on_an_empty_ternary_search_tree_get() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");

    if (res != NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove_2() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "different", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "good", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "mess", &test_int_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "different");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "good");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "mess");
    if (res == NULL) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "test", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res != NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "different");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "good");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "mess");
    if (res == NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove_3() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "testa", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "testab", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "testabc", &test_int_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testa");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testab");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testabc");
    if (res == NULL) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "test", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res != NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testa");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testab");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testabc");
    if (res == NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove_4() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "testabc", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "testab", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "testa", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "test", &test_int_el1);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "testabc");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testab");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testa");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res == NULL) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "testabc", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testabc");
    if (res != NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testab");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "testa");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test");
    if (res == NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_remove() {
    struct gnl_ternary_search_tree_t *ternary_search_tree = NULL;

    gnl_ternary_search_tree_put(&ternary_search_tree, "test_0", &test_int_el1);
    gnl_ternary_search_tree_put(&ternary_search_tree, "test_1", &test_int_el2);
    gnl_ternary_search_tree_put(&ternary_search_tree, "test_2", &test_int_el3);
    gnl_ternary_search_tree_put(&ternary_search_tree, "test_3", &test_int_el4);

    void *res = gnl_ternary_search_tree_get(ternary_search_tree, "test_0");
    if (res == NULL || *(int *)res != test_int_el1) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_1");
    if (res == NULL || *(int *)res != test_int_el2) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_2");
    if (res == NULL || *(int *)res != test_int_el3) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_3");
    if (res == NULL || *(int *)res != test_int_el4) {
        return -1;
    }

    int delete_res = gnl_ternary_search_tree_remove(ternary_search_tree, "test_1", NULL);
    if (delete_res != 0) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_1");
    if (res != NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_0");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_2");
    if (res == NULL) {
        return -1;
    }

    res = gnl_ternary_search_tree_get(ternary_search_tree, "test_3");
    if (res == NULL) {
        return -1;
    }

    gnl_ternary_search_tree_destroy(&ternary_search_tree, NULL);

    return 0;
}

int can_pass_null_ternary_search_tree() {
    gnl_ternary_search_tree_put(NULL, NULL, NULL);
    gnl_ternary_search_tree_get(NULL, NULL);
    gnl_ternary_search_tree_destroy(NULL, NULL);

    return 0;
}

int main() {
    gnl_printf_yellow("> struct gnl_ternary_search_tree_t tests:\n\n");

    gnl_assert(can_put_an_int, "can put an int element into a ternary_search_tree.");
    gnl_assert(can_remove_an_int, "can remove an int element from a ternary_search_tree.");

    gnl_assert(can_put_a_string, "can put a string element into a ternary_search_tree.");
    gnl_assert(can_remove_a_string, "can remove an string element from a ternary_search_tree.");

    gnl_assert(can_put_a_struct, "can put a struct element from a ternary_search_tree.");
    gnl_assert(can_remove_a_struct, "can remove a struct element from a ternary_search_tree.");

    gnl_assert(can_not_put_empty_key, "can not put an empty key to a ternary_search_tree.");
    gnl_assert(can_update_an_element, "can update an element into a ternary_search_tree.");

    gnl_assert(can_destroy_ternary_search_tree_complex_struct, "can destroy a complex struct elements from a ternary_search_tree.");
    gnl_assert(can_remove_an_allocated_pointer, "can remove an allocated pointer elements with no memory leaks from a ternary_search_tree.");

    gnl_assert(can_get_null_on_an_empty_ternary_search_tree_get, "can get null on empty ternary_search_tree get.");

    gnl_assert(can_pass_null_ternary_search_tree, "can give a null ternary_search_tree safely to the ternary_search_tree interface.");

    gnl_assert(can_remove_2, "can remove a unique key.");
    gnl_assert(can_remove_3, "can remove a prefix key of another longer key.");
    gnl_assert(can_remove_4, "can remove a key with at least one other key as prefix key.");

    gnl_assert(can_remove, "can remove a key.");

    // the gnl_ternary_search_tree_get method is implicitly tested by the other tests

    // the gnl_ternary_search_tree_destroy method is implicitly tested in every assertion


    printf("\n");
}