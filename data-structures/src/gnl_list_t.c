/*
 * This is a simple linked list implementation, it does not intend to be
 * exhaustive.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/gnl_list_t.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
int gnl_list_insert(struct gnl_list_t **list, void *el) {
    struct gnl_list_t *new_node = (struct gnl_list_t *)malloc(sizeof(struct gnl_list_t));

    GNL_NULL_CHECK(new_node, ENOMEM, -1)

    new_node->el = el;
    new_node->next = *list;

    *list = new_node;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_list_append(struct gnl_list_t **list, void *el) {
    struct gnl_list_t *new_node = (struct gnl_list_t *)malloc(sizeof(struct gnl_list_t));
    struct gnl_list_t *current = NULL;

    GNL_NULL_CHECK(new_node, ENOMEM, -1)

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

/**
 * {@inheritDoc}
 */
int gnl_list_search(struct gnl_list_t *list, const void *el, int (*compare)(const void *a, const void *b)) {
    struct gnl_list_t *current = list;

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

/**
 * {@inheritDoc}
 */
int gnl_list_delete(struct gnl_list_t **list, const void *el, int (*compare)(const void *a, const void *b), void (*destroy)(void *data)) {
    GNL_NULL_CHECK(list, EINVAL, -1)

    struct gnl_list_t *temp = *list;
    struct gnl_list_t *prev;

    if (temp != NULL && ((compare == NULL && temp->el == el) || (compare != NULL && compare(temp->el, el) == 0))) {
        *list = temp->next;

        if (destroy != NULL) {
            destroy(temp->el);
        }

        free(temp);

        return 0;
    }

    while (temp != NULL && ((compare == NULL && temp->el != el) || (compare != NULL && compare(temp->el, el) != 0))) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return 0;
    }

    prev->next = temp->next;

    if (destroy != NULL) {
        destroy(temp->el);
    }

    free(temp);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_list_destroy(struct gnl_list_t **list, void (*destroy)(void *data)) {
    GNL_NULL_CHECK(list, EINVAL, -1)

    struct gnl_list_t *current = *list;
    struct gnl_list_t *next;

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

#include <gnl_macro_end.h>