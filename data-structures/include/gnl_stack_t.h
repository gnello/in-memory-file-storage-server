
#ifndef GNL_STACK_H
#define GNL_STACK_H

/**
 * Holds the stack information.
 */
struct gnl_stack_t;

/**
 * Create a new stack.
 *
 * @return gnl_stack_t  Returns the new stack created on success,
 *                      NULL otherwise.
 */
extern struct gnl_stack_t *gnl_stack_init();

/**
 * Destroy a stack.
 *
 * @param s         The stack to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 */
extern void gnl_stack_destroy(struct gnl_stack_t *s, void (*destroy)(void *data));

/**
 * Put an element "el" into the stack "s".
 *
 * @param s     The stack where to push the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_stack_push(struct gnl_stack_t *s, void *el);

/**
 * Delete and return an element from the stack "s".
 *
 * @param s         The stack from where to pop the element.
 *
 * @return void*    Returns the element on success, if the
 *                  stack "s" is empty returns NULL.
 */
extern void *gnl_stack_pop(struct gnl_stack_t *s);

/**
 * Return the size of the stack "s".
 *
 * @param s     The stack from where to get the size.
 * @return int  Returns the stack size.
 */
extern unsigned long gnl_stack_size(const struct gnl_stack_t *s);

#endif //GNL_STACK_H