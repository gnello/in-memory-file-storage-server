#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_list_t.c"

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

    int el1 = 1;
    int el2 = 2;
    int el3 = 3;
    int el4 = 4;
    int el5 = 5;

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);
    gnl_list_insert(&list, &el4);
    gnl_list_insert(&list, &el5);

    int res = 0;
    if (*((int *)(list->el)) != el5) {
        res = -1;
    }

    gnl_list_destroy(&list);

    return res;
}

int can_append_int() {
    gnl_list_t *list = NULL;

    int el1 = 1;
    int el2 = 2;
    int el3 = 3;
    int el4 = 4;
    int el5 = 5;

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);
    gnl_list_insert(&list, &el4);

    gnl_list_append(&list, &el5);

    gnl_list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }

    int res = 0;
    if (*((int *)(current->el)) != el5) {
        res = -1;
    }

    gnl_list_destroy(&list);

    return res;
}

int can_find_an_int() {
    gnl_list_t *list = NULL;

    int el1 = 1;
    int el2 = 2;
    int el3 = 3;

    if (gnl_list_search(list, &el2, NULL) != 0) {
        return -1;
    }

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);

    int res = !gnl_list_search(list, &el2, NULL);

    gnl_list_destroy(&list);

    return res;
}

int can_delete_an_int() {
    gnl_list_t *list = NULL;

    int el1 = 1;
    int el2 = 2;
    int el3 = 3;
    int el4 = 4;
    int el5 = 5;

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);
    gnl_list_insert(&list, &el4);
    gnl_list_insert(&list, &el5);

    if (gnl_list_search(list, &el4, NULL) != 1) {
        return -1;
    }

    gnl_list_delete(&list, &el4);

    int res = gnl_list_search(list, &el4, NULL);

    gnl_list_destroy(&list);

    return res;
}

int can_insert_string() {
    gnl_list_t *list = NULL;

    char *el1 = "Lorem ipsum dolor sit amet";
    char *el2 = "consectetur adipiscing elit";
    char *el3 = "Nam posuere lectus sit amet malesuada dapibus";
    char *el4 = "Proin volutpat";
    char *el5 = "nisi at varius semper";

    gnl_list_insert(&list, el1);
    gnl_list_insert(&list, el2);
    gnl_list_insert(&list, el3);
    gnl_list_insert(&list, el4);
    gnl_list_insert(&list, el5);

    int res = 0;
    if (strcmp((char *)(list->el), el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list);

    return res;
}

int can_append_string() {
    gnl_list_t *list = NULL;

    char *el1 = "Lorem ipsum dolor sit amet";
    char *el2 = "consectetur adipiscing elit";
    char *el3 = "Nam posuere lectus sit amet malesuada dapibus";
    char *el4 = "Proin volutpat";
    char *el5 = "nisi at varius semper";

    gnl_list_insert(&list, el1);
    gnl_list_insert(&list, el2);
    gnl_list_insert(&list, el3);
    gnl_list_insert(&list, el4);

    gnl_list_append(&list, el5);

    gnl_list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }

    int res = 0;
    if (strcmp((char *)(current->el), el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list);

    return res;
}

int can_find_a_string() {
    gnl_list_t *list = NULL;

    char *el1 = "Lorem ipsum dolor sit amet";
    char *el2 = "consectetur adipiscing elit";
    char *el3 = "Nam posuere lectus sit amet malesuada dapibus";

    if (gnl_list_search(list, el2, NULL) != 0) {
        return -1;
    }

    gnl_list_insert(&list, el1);
    gnl_list_insert(&list, el2);
    gnl_list_insert(&list, el3);

    int res = !gnl_list_search(list, el2, NULL);

    gnl_list_destroy(&list);

    return res;
}

int can_delete_a_string() {
    gnl_list_t *list = NULL;

    char *el1 = "Lorem ipsum dolor sit amet";
    char *el2 = "consectetur adipiscing elit";
    char *el3 = "Nam posuere lectus sit amet malesuada dapibus";
    char *el4 = "Proin volutpat";
    char *el5 = "nisi at varius semper";

    gnl_list_insert(&list, el1);
    gnl_list_insert(&list, el2);
    gnl_list_insert(&list, el3);
    gnl_list_insert(&list, el4);
    gnl_list_insert(&list, el5);

    if (gnl_list_search(list, el4, NULL) != 1) {
        return -1;
    }

    gnl_list_delete(&list, el4);

    int res = gnl_list_search(list, el4, NULL);

    gnl_list_destroy(&list);

    return res;
}

int can_insert_struct() {
    gnl_list_t *list = NULL;

    struct test el1;
    el1.a = "Lorem ipsum dolor sit amet";
    el1.b = 1;

    struct test el2;
    el2.a = "consectetur adipiscing elit";
    el2.b = 2;

    struct test el3;
    el3.a = "Nam posuere lectus sit amet malesuada dapibus";
    el3.b = 3;

    struct test el4;
    el4.a = "Proin volutpat";
    el4.b = 4;

    struct test el5;
    el5.a = "nisi at varius semper";
    el5.b = 5;

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);
    gnl_list_insert(&list, &el4);
    gnl_list_insert(&list, &el5);

    int res = 0;
    if (test_cmp(list->el, (void *)&el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list);

    return res;
}

int can_append_struct() {
    gnl_list_t *list = NULL;

    struct test el1;
    el1.a = "Lorem ipsum dolor sit amet";
    el1.b = 1;

    struct test el2;
    el2.a = "consectetur adipiscing elit";
    el2.b = 2;

    struct test el3;
    el3.a = "Nam posuere lectus sit amet malesuada dapibus";
    el3.b = 3;

    struct test el4;
    el4.a = "Proin volutpat";
    el4.b = 4;

    struct test el5;
    el5.a = "nisi at varius semper";
    el5.b = 5;

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);
    gnl_list_insert(&list, &el4);

    gnl_list_append(&list, &el5);

    gnl_list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }

    int res = 0;
    if (test_cmp(current->el, (void *)&el5) != 0) {
        res = -1;
    }

    gnl_list_destroy(&list);

    return res;
}

int can_find_struct() {
    gnl_list_t *list = NULL;

    struct test el1;
    el1.a = "Lorem ipsum dolor sit amet";
    el1.b = 1;

    struct test el2;
    el2.a = "consectetur adipiscing elit";
    el2.b = 2;

    struct test el3;
    el3.a = "Nam posuere lectus sit amet malesuada dapibus";
    el3.b = 3;

    if (gnl_list_search((void *)list, (void *)&el2, test_cmp) != 0) {
        return -1;
    }

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);

    int res = !gnl_list_search((void *)list, (void *)&el2, test_cmp);

    gnl_list_destroy(&list);

    return res;
}

int can_delete_a_struct() {
    gnl_list_t *list = NULL;

    struct test el1;
    el1.a = "Lorem ipsum dolor sit amet";
    el1.b = 1;

    struct test el2;
    el2.a = "consectetur adipiscing elit";
    el2.b = 2;

    struct test el3;
    el3.a = "Nam posuere lectus sit amet malesuada dapibus";
    el3.b = 3;

    struct test el4;
    el4.a = "Proin volutpat";
    el4.b = 4;

    struct test el5;
    el5.a = "nisi at varius semper";
    el5.b = 5;

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);
    gnl_list_insert(&list, &el4);
    gnl_list_insert(&list, &el5);

    if (gnl_list_search((void *)list, (void *)&el4, test_cmp) != 1) {
        return -1;
    }

    gnl_list_delete(&list, &el4);

    int res = gnl_list_search((void *)list, (void *)&el4, test_cmp);

    gnl_list_destroy(&list);

    return res;
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

    // the gnl_list_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}