
#ifndef GNL_TS_NB_QUEUE_H
#define GNL_TS_NB_QUEUE_H

/**
 * Holds the thread-safe non-blocking queue information.
 */
struct gnl_ts_nb_queue_t;

/**
 * Create a new thread-safe non-blocking queue.
 *
 * @return queue_t  Returns the new thread-safe non-blocking
 *                  queue created on success, NULL otherwise.
 */
extern struct gnl_ts_nb_queue_t *gnl_ts_nb_queue_init();

/**
 * Destroy a thread-safe non-blocking queue.
 * This is not an atomic operation! Call this method only in
 * a main thread where you are sure there are not other threads
 * using the queue q.
 *
 * @param q         The queue to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_nb_queue_destroy(struct gnl_ts_nb_queue_t *q, void (*destroy)(void *data));

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to enqueue the element.
 * @param el    The element.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_nb_queue_enqueue(struct gnl_ts_nb_queue_t *q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q The queue from where to dequeue the element.
 *
 * @return  Returns the element on success, if the
 *          queue "q" is empty returns NULL.
 */
extern void *gnl_ts_nb_queue_dequeue(struct gnl_ts_nb_queue_t *q);

/**
 * Return the size of the queue "q".
 *
 * @param q The queue from where to get the size.
 *
 * @return  Returns the queue size.
 */
extern unsigned long gnl_ts_nb_queue_size(struct gnl_ts_nb_queue_t *q);

#endif //GNL_TS_NB_QUEUE_H