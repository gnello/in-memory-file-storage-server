/*
 * This is a simple ternary search tree implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdlib.h>
#include <errno.h>
#include "../include/gnl_ternary_search_tree_t.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 *
 * key      The key of the element.
 * data     The value of the element.
 * left     The left child, less than the key.
 * right    The right child, greater than the key.
 * mid      The left child, equal than the key.
 */
struct gnl_ternary_search_tree_t {
    char key;
    void *data;
    struct gnl_ternary_search_tree_t* left;
    struct gnl_ternary_search_tree_t* right;
    struct gnl_ternary_search_tree_t* mid;
};

/**
 * {@inheritDoc}
 */
void gnl_ternary_search_tree_destroy(struct gnl_ternary_search_tree_t *t, void (*destroy)(void *data)) {
    if (t != NULL) {
        // delete the left tree
        gnl_ternary_search_tree_destroy(t->left, destroy);

        // delete the middle tree
        gnl_ternary_search_tree_destroy(t->mid, destroy);

        // delete the right tree
        gnl_ternary_search_tree_destroy(t->right, destroy);

        // delete the root tree
        if (t->data != NULL && destroy != NULL) {
            destroy(t->data);
        }

        free(t);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_ternary_search_tree_put(struct gnl_ternary_search_tree_t **t, char *key, void *el) {
    struct gnl_ternary_search_tree_t *new_node = (struct gnl_ternary_search_tree_t *)malloc(sizeof(struct gnl_ternary_search_tree_t));
    GNL_NULL_CHECK(new_node, ENOMEM, -1)

    // initialize the new node
    new_node->key = *key;
    new_node->data = NULL;
    new_node->left = NULL;
    new_node->mid = NULL;
    new_node->right = NULL;

    // if the root tree is empty add the node
    if (*t == NULL) {
        *t = new_node;
    }

    // if the given key is less than the tree key pick the left node
    if ((*key) < (*t)->key) {
        return gnl_ternary_search_tree_put(&((*t)->left), key, el);
    }
    // if the given key is greater than the tree key pick the right node
    else if ((*key) > (*t)->key) {
        return gnl_ternary_search_tree_put(&((*t)->right), key, el);
    }
    // if the given key is equal than the tree key...
    else
    {
        // if the given key is ended store the data into the node
        if (*(key + 1) == '\0') {
            (*t)->data = el;
        }
        // otherwise, pick the middle node
        else {
            return gnl_ternary_search_tree_put(&((*t)->mid), key + 1, el);
        }
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
void *gnl_ternary_search_tree_get(struct gnl_ternary_search_tree_t *t, char *key) {
    GNL_NULL_CHECK(t, EINVAL, NULL)

    // if the given key is less than the tree key pick the left node
    if (*key < t->key) {
        return gnl_ternary_search_tree_get(t->left, key);
    }
    // if the given key is greater than the tree key pick the right node
    else if (*key > t->key) {
        return gnl_ternary_search_tree_get(t->right, key);
    }
    // if the given key is equal than the tree key...
    else
    {
        // if the given key is ended return the data of the node
        if (*(key + 1) == '\0') {
            return t->data;
        }

        // otherwise, pick the middle node
        return gnl_ternary_search_tree_get(t->mid, key + 1);
    }
}

/**
 * {@inheritDoc}
 */
void *gnl_ternary_search_tree_remove(struct gnl_ternary_search_tree_t *t, char *key) {
    return NULL;
}

#include <gnl_macro_end.h>