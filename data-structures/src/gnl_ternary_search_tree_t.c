/*
 * This is a simple ternary search tree implementation, it does not intend to be exhaustive.
 */

#include <stdlib.h>
#include <string.h>
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
void gnl_ternary_search_tree_destroy(struct gnl_ternary_search_tree_t **t, void (*destroy)(void *data)) {
    if (t == NULL || *t == NULL) {
        return;
    }

    if ((*t)->left != NULL) {
        // delete the left tree
        gnl_ternary_search_tree_destroy(&((*t)->left), destroy);
    }

    if ((*t)->mid != NULL) {
        // delete the middle tree
        gnl_ternary_search_tree_destroy(&((*t)->mid), destroy);
    }

    if ((*t)->right != NULL) {
        // delete the right tree
        gnl_ternary_search_tree_destroy(&((*t)->right), destroy);
    }

    // delete the root tree
    if (destroy != NULL) {
        destroy((*t)->data);
    }

    free(*t);
}

/**
 * {@inheritDoc}
 */
int gnl_ternary_search_tree_put(struct gnl_ternary_search_tree_t **t, const char *key, void *el) {
    GNL_NULL_CHECK(key, EINVAL, -1)

    // check if the key is a valid string
    GNL_MINUS1_CHECK(-1 * (strlen(key) == 0), EINVAL, -1)

    // if the root tree is empty add a new node
    if (*t == NULL) {
        struct gnl_ternary_search_tree_t *new_node = (struct gnl_ternary_search_tree_t *)malloc(sizeof(struct gnl_ternary_search_tree_t));
        GNL_NULL_CHECK(new_node, ENOMEM, -1)

        // initialize the new node
        new_node->key = *key;
        new_node->data = NULL;
        new_node->left = NULL;
        new_node->mid = NULL;
        new_node->right = NULL;

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
    else {
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
void *gnl_ternary_search_tree_get(struct gnl_ternary_search_tree_t *t, const char *key) {
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
    else {
        // if the given key is ended return the data of the node
        if (*(key + 1) == '\0') {
            return t->data;
        }

        // otherwise, pick the middle node
        return gnl_ternary_search_tree_get(t->mid, key + 1);
    }
}

/**
 * Return whether the given node has a child or not.
 *
 * @param t The ternary_search_tree from where to get the element.
 *
 * @return  Returns 1 if the given node has a child, 0 otherwise.
 */
static int has_child(struct gnl_ternary_search_tree_t *t) {
    GNL_NULL_CHECK(t, EINVAL, -1)

    if (t->left != NULL) {
        return 1;
    }

    if (t->mid != NULL) {
        return 1;
    }

    if (t->right != NULL) {
        return 1;
    }

    return 0;
}

/**
 * Strongly inspired from https://www.geeksforgeeks.org/ternary-search-tree-deletion/
 *
 * Recursively remove the nodes of a key when necessary:
 * - case 1:    if the given key may not be present into the given tree then the tree is left untouched.
 * - case 2:    if the given key is unique in the tree then remove all his nodes.
 * - case 3:    if the given key is a prefix key of another longer key then remove only his data.
 * - case 4:    if the given key has at least one other key as prefix key then delete nodes from
 *              the end of the given key until the end node of the longest prefix key.
 *
 * @param t         The ternary_search_tree from where to remove the element.
 * @param key       The key of the element to remove.
 * @param destroy   The destroy function to free pointer data,
 *                  if NULL is passed, no free will be performed.
 *
 * @return          Returns 1 if the current node is not a leaf nor has it a child,
 *                  0 otherwise.
 */
static int remove_node(struct gnl_ternary_search_tree_t *t, const char *key, void (*destroy)(void *data)) {

    // case 3: the given key is a prefix key of another longer key
    if (*key < t->key) {
        remove_node(t->left, key, destroy);
    }
    else if (*key > t->key) {
        remove_node(t->right, key, destroy);
    }
    // case 1: the given key may not be present into the given tree
    else {
        // case 4: the given key has at least one other key as prefix key
        if (*(key + 1) == '\0') {
            // remove the data if present
            if (t->key == *key && t->data != NULL) {
                if (destroy != NULL) {
                    destroy(t->data);
                }

                t->data = NULL;

                // return 1 if the current node does not have any children node
                return !has_child(t);
            }
            // the given key is not present in the tree
            else {
                return 0;
            }
        } else {
            // case 2: the given key is unique in the tree
            if (remove_node(t->mid, key + 1, destroy)) {
                // delete the last node, it has no children, nor it is part of any other string
                free(t->mid);
                t->mid = NULL;

                return t->data == NULL && !has_child(t);
            }
        }
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_ternary_search_tree_remove(struct gnl_ternary_search_tree_t *t, const char *key, void (*destroy)(void *data)) {
    GNL_NULL_CHECK(t, EINVAL, -1)

    remove_node(t, key, destroy);

    return 0;
}

#include <gnl_macro_end.h>