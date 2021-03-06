
#ifndef GNL_QUEUE_H
#define GNL_QUEUE_H

/**
 * Holds the queue information.
 */
struct gnl_queue_t;

/**
 * Create a new queue.
 *
 * @return gnl_queue_t  Returns the new queue created on success,
 *                      NULL otherwise.
 */
extern struct gnl_queue_t *gnl_queue_init();

/**
 * Destroy a queue.
 *
 * @param q         The queue to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 */
extern void gnl_queue_destroy(struct gnl_queue_t *q, void (*destroy)(void *data));

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to enqueue the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_queue_enqueue(struct gnl_queue_t *q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q         The queue from where to dequeue the element.
 *
 * @return void*    Returns the element on success, if the
 *                  queue "q" is empty returns NULL.
 */
extern void *gnl_queue_dequeue(struct gnl_queue_t *q);

/**
 * Return the size of the queue "q".
 *
 * @param q     The queue from where to get the size.
 * @return int  Returns the queue size.
 */
extern unsigned long gnl_queue_size(const struct gnl_queue_t *q);

#endif //GNL_QUEUE_H