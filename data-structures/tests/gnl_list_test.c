#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_list_t.c"

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

    if (gnl_list_search(list, &el2) != 0) {
        return -1;
    }

    gnl_list_insert(&list, &el1);
    gnl_list_insert(&list, &el2);
    gnl_list_insert(&list, &el3);

    int res = !gnl_list_search(list, &el2);

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

    if (gnl_list_search(list, &el4) != 1) {
        return -1;
    }

    gnl_list_delete(&list, &el4);

    int res = gnl_list_search(list, &el4);

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

    if (gnl_list_search(list, el2) != 0) {
        return -1;
    }

    gnl_list_insert(&list, el1);
    gnl_list_insert(&list, el2);
    gnl_list_insert(&list, el3);

    int res = !gnl_list_search(list, el2);

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

    if (gnl_list_search(list, el4) != 1) {
        return -1;
    }

    gnl_list_delete(&list, el4);

    int res = gnl_list_search(list, el4);

    gnl_list_destroy(&list);

    return res;
}

int main() {
    gnl_printf_yellow("> gnl_list tests:\n\n");

    gnl_assert(can_insert_int, "can insert an int element at the beginning of the list.");
    gnl_assert(can_append_int, "can append an int element at the end of the list.");
    gnl_assert(can_find_an_int, "can check whether an int element is present into the list.");
    gnl_assert(can_delete_an_int, "can delete an int element from the list.");

    gnl_assert(can_insert_string, "can insert a string element at the beginning of the list.");
    gnl_assert(can_append_string, "can append a string element at the end of the list.");
    gnl_assert(can_find_a_string, "can check whether a string element is present into the list.");
    gnl_assert(can_delete_a_string, "can delete a string element from the list.");

    // the gnl_list_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}