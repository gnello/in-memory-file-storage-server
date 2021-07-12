
#ifndef GNL_TS_QUEUE_H
#define GNL_TS_QUEUE_H

/**
 * Holds the thread-safe blocking bounded queue information.
 */
struct gnl_ts_bb_queue_t;

/**
 * Create a new thread-safe blocking bounded queue.
 *
 * @param bound     The max number of items storable in the queue.
 *                  It must be >= 0.
 *
 * @return queue_t  Returns the new thread-safe blocking bounded
 *                  queue created on success, NULL otherwise.
 */
extern struct gnl_ts_bb_queue_t *gnl_ts_bb_queue_init(int bound);

/**
 * Destroy a thread-safe blocking bounded queue.
 * CAUTION!!! This is not an atomic operation! Call this method
 * only in a main thread where you are sure there are not other
 * threads using the queue q.
 *
 * @param q         The queue to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return int      Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_bb_queue_destroy(struct gnl_ts_bb_queue_t *q, void (*destroy)(void *data));

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to enqueue the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_bb_queue_enqueue(struct gnl_ts_bb_queue_t *q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q         The queue from where to dequeue the element.
 *
 * @return void*    Returns the element on success, if the
 *                  queue "q" is empty returns NULL.
 */
extern void *gnl_ts_bb_queue_dequeue(struct gnl_ts_bb_queue_t *q);

/**
 * Return the size of the queue "q".
 *
 * @param q     The queue from where to get the size.
 *
 * @return int  Returns the queue size.
 */
extern unsigned long gnl_ts_bb_queue_size(struct gnl_ts_bb_queue_t *q);

#endif //GNL_TS_QUEUE_H