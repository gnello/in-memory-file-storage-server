
#ifndef GNL_QUEUE_H
#define GNL_QUEUE_H

/**
 * Holds the queue information.
 *
 * struct gnl_queue_t {
 *     struct gnl_queue_node *front;
 *     struct gnl_queue_node *rear;
 *     unsigned long size;
 * };
 *
 * front    The head pointer of the queue.
 * rear     The tail pointer of the queue.
 * size     The size of the queue.
 */
typedef struct gnl_queue_t gnl_queue_t;

/**
 * Create a new queue.
 *
 * @return gnl_queue_t  Returns the new queue created on success,
 *                      NULL otherwise.
 */
extern gnl_queue_t *gnl_queue_init();

/**
 * Destroy a queue.
 *
 * @param q The queue to be destroyed.
 */
extern void gnl_queue_destroy(gnl_queue_t *q);

/**
 * Put an element "el" into the queue "q".
 *
 * @param q     The queue where to enqueue the element.
 * @param el    The element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
extern int gnl_queue_enqueue(gnl_queue_t *q, void *el);

/**
 * Delete and return an element from the queue "q".
 *
 * @param q         The queue from where to dequeue the element.
 *
 * @return void*    Returns the element on success, if the
 *                  queue "q" is empty returns NULL.
 */
extern void *gnl_queue_dequeue(gnl_queue_t *q);

/**
 * Return the size of the queue "q".
 *
 * @param q     The queue from where to get the size.
 * @return int  Returns the queue size.
 */
extern unsigned long gnl_queue_size(const gnl_queue_t *q);

#endif //GNL_QUEUE_H