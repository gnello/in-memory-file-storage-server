
#ifndef GNL_TS_STACK_H
#define GNL_TS_STACK_H

/**
 * Holds the thread-safe stack information.
 */
struct gnl_ts_bb_stack_t;

/**
 * Create a new thread-safe blocking bounded  stack.
 *
 * @param bound     The max number of items storable in the stack.
 *                  It must be >= 0.
 *
 * @return stack_t  Returns the new thread-safe stack created on success,
 *                  NULL otherwise.
 */
extern struct gnl_ts_bb_stack_t *gnl_ts_bb_stack_init(int bound);

/**
 * Destroy a thread-safe blocking bounded stack.
 * CAUTION!!! This is not an atomic operation! Call this method
 * only in a main thread where you are sure there are not other
 * threads using the stack s.
 *
 * @param s     The stack to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_bb_stack_destroy(struct gnl_ts_bb_stack_t *s, void (*destroy)(void *data));

/**
 * Put an element "el" into the thread-safe blocking bounded stack "s".
 *
 * @param s     The stack where to enstack the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_bb_stack_push(struct gnl_ts_bb_stack_t *s, void *el);

/**
 * Delete and return an element from the thread-safe blocking bounded stack "s".
 *
 * @param s         The stack from where to destack the element.
 *
 * @return void*    Returns the element on success, if the
 *                  stack "s" is empty returns NULL.
 */
extern void *gnl_ts_bb_stack_pop(struct gnl_ts_bb_stack_t *s);

/**
 * Return the size of the thread-safe blocking bounded stack "s".
 *
 * @param s     The stack from where to get the size.
 * @return int  Returns the stack size.
 */
extern unsigned long gnl_ts_bb_stack_size(struct gnl_ts_bb_stack_t *s);

#endif //GNL_TS_STACK_H