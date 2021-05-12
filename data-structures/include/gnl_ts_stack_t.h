
#ifndef GNL_TS_STACK_H
#define GNL_TS_STACK_H

/**
 * Holds the thread-safe stack information.
 *
 * struct gnl_ts_stack_t {
 *     pthread_mutex_t mtx;
 *     gnl_stack_t *s;
 * };
 *
 * mtx  The mutex var of the thread-safe stack.
 * s    The stack data-structure.
 */
typedef struct gnl_ts_stack_t gnl_ts_stack_t;

/**
 * Create a new thread-safe stack.
 *
 * @return stack_t  Returns the new thread-safe stack created on success,
 *                  NULL otherwise.
 */
extern gnl_ts_stack_t *gnl_ts_stack_init();

/**
 * Destroy a thread-safe stack.
 *
 * @param s     The stack to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_stack_destroy(gnl_ts_stack_t *s, void (*destroy)(void *data));

/**
 * Put an element "el" into the stack "s".
 *
 * @param s     The stack where to enstack the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_stack_push(gnl_ts_stack_t *s, void *el);

/**
 * Delete and return an element from the stack "s".
 *
 * @param s         The stack from where to destack the element.
 *
 * @return void*    Returns the element on success, if the
 *                  stack "s" is empty returns NULL.
 */
extern void *gnl_ts_stack_pop(gnl_ts_stack_t *s);

/**
 * Return the size of the stack "s".
 *
 * @param s     The stack from where to get the size.
 * @return int  Returns the stack size.
 */
extern unsigned long gnl_ts_stack_size(gnl_ts_stack_t *s);

#endif //GNL_TS_STACK_H