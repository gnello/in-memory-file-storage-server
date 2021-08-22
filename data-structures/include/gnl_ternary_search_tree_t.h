
#ifndef GNL_TERNARY_SEARCH_TREE_H
#define GNL_TERNARY_SEARCH_TREE_H

/**
 * Holds the ternary_search_tree information.
 */
struct gnl_ternary_search_tree_t;

/**
 * Destroy a ternary_search_tree.
 *
 * @param t         The ternary_search_tree to be destroyed.
 * @param destroy   The destroy function to free pointer data,
 *                  if NULL is passed, no free will be performed.
 */
extern void gnl_ternary_search_tree_destroy(struct gnl_ternary_search_tree_t **t, void (*destroy)(void *data));

/**
 * Put an element "el" into the ternary_search_tree "t". A second put of the same key
 * will update the target el.
 *
 * @param t     The ternary_search_tree where to put the element.
 * @param key   The key of the element to put.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ternary_search_tree_put(struct gnl_ternary_search_tree_t **t, const char *key, void *el);

/**
 * Return an element from the ternary_search_tree "t".
 *
 * @param t         The ternary_search_tree from where to get the element.
 * @param key       The key of the element to get.
 *
 * @return void*    Returns the element on success, if the
 *                  element is not found returns NULL.
 */
extern void *gnl_ternary_search_tree_get(struct gnl_ternary_search_tree_t *t, const char *key);

/**
 * Remove an element from the ternary_search_tree "t".
 *
 * @param t         The ternary_search_tree from where to remove the element.
 * @param key       The key of the element to remove.
 * @param destroy   The destroy function to free pointer data,
 *                  if NULL is passed, no free will be performed.
 *
 * @return void*    Returns 0 on success, -1 otherwise
 */
extern int gnl_ternary_search_tree_remove(struct gnl_ternary_search_tree_t *t, const char *key, void (*destroy)(void *data));

#endif //GNL_TERNARY_SEARCH_TREE_H