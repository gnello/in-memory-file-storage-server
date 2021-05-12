
#ifndef GNL_TS_QUEUE_H
#define GNL_TS_QUEUE_H

/**
 * Holds the thread-safe queue information.
 *
 * struct gnl_ts_queue_t {
 *     pthread_mutex_t mtx;
 *     gnl_queue_t *q;
 * };
 *
 * mtx  The mutex var of the thread-safe queue.
 * q    The queue data-structure.
 */
typedef struct gnl_ts_queue_t gnl_ts_queue_t;

/**
 * Create a new thread-safe queue.
 *
 * @return queue_t  Returns the new thread-safe queue created on success,
 *                  NULL otherwise.
 */
extern gnl_ts_queue_t *gnl_ts_queue_init();

/**
 * Destroy a thread-safe queue.
 *
 * @param q         The queue to be destroyed.
 * @param destroy   The destroy function to free complex data,
 *                  if NULL is passed a standard free will be performed.
 *
 * @return int      Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_queue_destroy(gnl_ts_queue_t *q, void (*destroy)(void *data));

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to enqueue the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_ts_queue_enqueue(gnl_ts_queue_t *q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q         The queue from where to dequeue the element.
 *
 * @return void*    Returns the element on success, if the
 *                  queue "q" is empty returns NULL.
 */
extern void *gnl_ts_queue_dequeue(gnl_ts_queue_t *q);

/**
 * Return the size of the queue "q".
 *
 * @param q     The queue from where to get the size.
 * @return int  Returns the queue size.
 */
extern unsigned long gnl_ts_queue_size(gnl_ts_queue_t *q);

#endif //GNL_TS_QUEUE_H