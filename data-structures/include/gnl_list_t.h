
#ifndef GNL_LIST_H
#define GNL_LIST_H

/**
 * Holds the list information.
 *
 * el   The element of the list node.
 * next The pointer to the next element of the list.
 */
struct gnl_list_t {
    void *el;
    struct gnl_list_t *next;
};

/**
 * Insert the element "el" onto the back of a linked list "list".
 *
 * @param list  The list where append the element "el".
 * @param el    The element to be appended.
 *
 * @return int Returns 0 on success, -1 otherwise.
 */
extern int gnl_list_insert(struct gnl_list_t **list, void *el);

/**
 * Insert an element "el" onto the back of a linked list "list".
 *
 * @param list  The list where append the element "el".
 * @param el    The element to be appended.
 *
 * @return int Returns 0 on success, -1 otherwise.
 */
extern int gnl_list_append(struct gnl_list_t **list, void *el);

/**
 * Check whether the element "el" is present into the list.
 *
 * @param list      The list where to search fot the element "el".
 * @param el        The element to be searched.
 * @param compare   The eventual function to use to make the comparison,
 *                  if NULL is passed a standard comparison will be performed.
 *
 * @return int      Returns 1 if the element is present, 0 otherwise.
 */
extern int gnl_list_search(struct gnl_list_t *list, const void *el, int (*compare)(const void *a, const void *b));

/**
 * Remove an element "el" from a linked list "list".
 *
 * @param list      The list from where delete the element "el".
 * @param el        The element to be deleted.
 * @param compare   The eventual function to use to make the comparison to find
 *                  the element to delete. If NULL is passed a standard comparison
 *                  will be performed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return int Returns always 0 (for now...).
 */
extern int gnl_list_delete(struct gnl_list_t **list, const void *el, int (*compare)(const void *a, const void *b),
        void (*destroy)(void *data));

/**
 * Destroy the entire list.
 *
 * @param list      The list to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return          Returns always 0 (for now...).
 */
extern int gnl_list_destroy(struct gnl_list_t **list, void (*destroy)(void *data));

#endif //GNL_LIST_H