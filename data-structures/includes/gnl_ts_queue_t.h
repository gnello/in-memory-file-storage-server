
#ifndef GNL_TS_QUEUE_H
#define GNL_TS_QUEUE_H

extern const int GNL_TS_QUEUE_FIFO;
extern const int GNL_TS_QUEUE_LIFO;

typedef struct gnl_ts_queue_t gnl_ts_queue_t;

/**
 * Create a new thread-safe queue.
 *
 * @param type      The type of the queue. Possible values:
 *                  GNL_QUEUE_FIFO, GNL_QUEUE_LIFO.
 *
 * @return queue_t  Returns the new thread-safe queue created on success,
 *                  NULL otherwise.
 */
extern gnl_ts_queue_t *gnl_ts_queue_init(int type);

/**
 * Destroy a thread-safe queue.
 *
 * @param q     The queue to be destroyed.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_queue_destroy(gnl_ts_queue_t **q);

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to put the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_queue_push(gnl_ts_queue_t **q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q         The queue from where to pop the element.
 * @param el        The pointer where to put the popped the element.
 *
 * @return void*    Returns the element on success, if the
 *                  queue "q" is empty returns NULL.
 */
extern void *gnl_ts_queue_pop(gnl_ts_queue_t **q);

/**
 * Return the size of the queue "q".
 *
 * @param q     The queue from where to get the size.
 * @return int  Returns the queue size.
 */
extern int gnl_ts_queue_size(const gnl_ts_queue_t *q);

#endif //GNL_TS_QUEUE_H