/*
 * This is a simple linked list implementation, it does not intend to be
 * exhaustive but it's nice :)
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct gnl_list {
    void *el;
    struct gnl_list *next;
} gnl_list_t;

int gnl_list_insert(gnl_list_t **list, void *el) {
    gnl_list_t *new_node = (gnl_list_t*)malloc(sizeof(gnl_list_t));

    if (new_node == NULL) {
        perror("malloc");

        return -1;
    }

    new_node->el = el;
    new_node->next = (*list);

    (*list) = new_node;

    return 0;
}

int gnl_list_append(gnl_list_t **list, void *el) {
    gnl_list_t *new_node = (gnl_list_t*)malloc(sizeof(gnl_list_t));
    gnl_list_t *current = NULL;

    if (new_node == NULL) {
        perror("malloc");

        return -1;
    }

    new_node->el = el;
    new_node->next = NULL;

    if (*list == NULL) {
        *list = new_node;
        return 0;
    }

    current = *list;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_node;

    return 0;
}

int gnl_list_search(gnl_list_t *list, const void *el) {
    gnl_list_t *current = list;

    while (current != NULL) {
        if (current->el == el) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

int gnl_list_delete(gnl_list_t **list, const void *el) {
    gnl_list_t *temp = *list;
    gnl_list_t *prev;

    if (temp != NULL && temp->el == el) {
        *list = temp->next;
        free(temp);

        return 0;
    }

    while (temp != NULL && temp->el != el) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return 0;
    }

    prev->next = temp->next;
    free(temp);

    return 0;
}

int gnl_list_destroy(gnl_list_t **list) {
    gnl_list_t *current = *list;
    gnl_list_t *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *list = NULL;

    return 0;
}