
#ifndef GNL_MIN_HEAP_H
#define GNL_MIN_HEAP_H

/**
 * Holds the min heap information.
 *
 * struct gnl_min_heap_t {
 *     unsigned long size;
 *     struct gnl_min_heap_node *list;
 * };
 *
 * size The size of the min heap.
 * list The pointer to the list elements of the min heap.
 */
typedef struct gnl_min_heap_t gnl_min_heap_t;

/**
 * Create a new min heap.
 *
 * @return gnl_min_heap_t   Returns the new min heap created on success,
 *                          NULL otherwise.
 */
extern gnl_min_heap_t *gnl_min_heap_init();

/**
 * Destroy a min heap.
 *
 * @param mh    The min heap to be destroyed.
 */
extern void gnl_min_heap_destroy(gnl_min_heap_t *mh);

/**
 * Insert an element "el" into the min heap "mh".
 *
 * @param mh    The min heap where to insert the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_min_heap_insert(gnl_min_heap_t *mh, void *el, int key);

/**
 * Delete and return an element from the in heap "mh".
 *
 * @param mh        The min heap from where to extract the element.
 *
 * @return void*    Returns the element on success, if the
 *                  min heap "mh" is empty returns NULL.
 */
extern void *gnl_min_heap_extract_min(gnl_min_heap_t *mh);

/**
 * Decrease the key of the i-element of the min heap by the given "key" value.
 *
 * @param mh    The min heap from where to decrease the key of a element.
 * @param i     The index of the element into the min heap.
 * @param key   The new key value of the element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_min_heap_decrease_key(gnl_min_heap_t *mh, int i, int key);

#endif //GNL_MIN_HEAP_H