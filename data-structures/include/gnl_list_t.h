
#ifndef GNL_LIST_H
#define GNL_LIST_H

/**
 * Holds the list information.
 *
 * struct gnl_list_t {
 *     void *el;
 *     struct gnl_list_t *next;
 * };
 *
 * el   The element of the list node.
 * next The pointer to the next element of the list.
 */
typedef struct gnl_list_t gnl_list_t;

/**
 * Insert the element "el" onto the back of a linked list "list".
 *
 * @param list  The list where append the element "el".
 * @param el    The element to be appended.
 *
 * @return int Returns 0 on success, -1 otherwise.
 */
extern int gnl_list_insert(gnl_list_t **list, void *el);

/**
 * Insert an element "el" onto the back of a linked list "list".
 *
 * @param list  The list where append the element "el".
 * @param el    The element to be appended.
 *
 * @return int Returns 0 on success, -1 otherwise.
 */
extern int gnl_list_append(gnl_list_t **list, void *el);

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
extern int gnl_list_search(gnl_list_t *list, const void *el, int (*compare)(const void *a, const void *b));

/**
 * Remove an element "el" from a linked list "list".
 *
 * @param list The list from where delete the element "el".
 * @param el   The element to be deleted.
 *
 * @return int Returns always 0 (for now...).
 */
extern int gnl_list_delete(gnl_list_t **list, const void *el);

/**
 * Destroy the entire list.
 *
 * @param list      The list to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return          Returns always 0 (for now...).
 */
extern int gnl_list_destroy(gnl_list_t **list, void (*destroy)(void *data));

#endif //GNL_LIST_H