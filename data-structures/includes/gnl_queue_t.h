
#ifndef GNL_QUEUE_H
#define GNL_QUEUE_H

typedef struct gnl_queue_t gnl_queue_t;

extern const int GNL_FIFO_QUEUE;
extern const int GNL_LIFO_QUEUE;

/**
 * Create a new queue.
 *
 * @param type          The type of the queue. Possible values:
 *                      GNL_FIFO_QUEUE, GNL_LIFO_QUEUE.
 *
 * @return gnl_queue_t  Returns the new queue created on success,
 *                      NULL otherwise.
 */
extern gnl_queue_t *gnl_queue_init(int type);

/**
 * Destroy a queue.
 *
 * @param q The queue to be destroyed.
 */
extern void gnl_queue_destroy(gnl_queue_t **q);

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to put the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_queue_push(gnl_queue_t **q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q         The queue from where to pop the element.
 * @param el        The pointer where to put the popped the element.
 *
 * @return void*    Returns the element on success, if the
 *                  queue "q" is empty returns NULL.
 */
extern void *gnl_queue_pop(gnl_queue_t **q);

/**
 * Return the size of the queue "q".
 *
 * @param q     The queue from where to get the size.
 * @return int  Returns the queue size.
 */
extern int gnl_queue_size(const gnl_queue_t *q);

#endif //GNL_QUEUE_H