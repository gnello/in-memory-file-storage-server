/*
 * This is a simple linked list implementation, it does not intend to be
 * exhaustive but it's nice :)
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/gnl_list_t.h"

#define NULL_VALIDATOR(list, error_code, return_code) {     \
    if (list == NULL) {                                     \
        errno = error_code;                                 \
                                                            \
        return return_code;                                 \
    }                                                       \
}

struct gnl_list_t {
    void *el;
    struct gnl_list_t *next;
};

int gnl_list_insert(gnl_list_t **list, void *el) {
    gnl_list_t *new_node = (struct gnl_list_t *)malloc(sizeof(struct gnl_list_t));

    NULL_VALIDATOR(new_node, ENOMEM, -1)

    new_node->el = el;
    new_node->next = *list;

    *list = new_node;

    return 0;
}

int gnl_list_append(gnl_list_t **list, void *el) {
    gnl_list_t *new_node = (struct gnl_list_t *)malloc(sizeof(struct gnl_list_t));
    gnl_list_t *current = NULL;

    NULL_VALIDATOR(new_node, ENOMEM, -1)

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

int gnl_list_search(gnl_list_t *list, const void *el, int (*compare)(const void *a, const void *b)) {
    gnl_list_t *current = list;

    while (current != NULL) {
        if (compare == NULL) {
            if (current->el == el) {
                return 1;
            }
        } else {
            if (compare(current->el, el) == 0) {
                return 1;
            }
        }
        current = current->next;
    }

    return 0;
}

int gnl_list_delete(gnl_list_t **list, const void *el) {
    NULL_VALIDATOR(list, EINVAL, -1)

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

int gnl_list_destroy(gnl_list_t **list, void (*destroy)(void *data)) {
    NULL_VALIDATOR(list, EINVAL, -1)

    gnl_list_t *current = *list;
    gnl_list_t *next;

    while (current != NULL) {
        next = current->next;

        if (destroy != NULL) {
            destroy(current->el);
        }

        free(current);
        current = next;
    }

    *list = NULL;

    return 0;
}

#undef NULL_VALIDATOR