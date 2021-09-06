
#ifndef GNL_MESSAGE_NQ_H
#define GNL_MESSAGE_NQ_H

#include <gnl_queue_t.h>
#include "./gnl_message_snb.h"

/**
 * The message_nq message.
 */
struct gnl_message_nq {
    int number;
    struct gnl_queue_t *queue;
};

/**
 * Create a new message_nq.
 *
 * @return The message_nq struct created.
 */
extern struct gnl_message_nq *gnl_message_nq_init();

/**
 * Destroy the given message_nq.
 *
 * @param message_nq    The message_nq to be destroyed.
 */
extern void gnl_message_nq_destroy(struct gnl_message_nq *message_nq);

/**
 * Encode the message_nq and put it into "dest".
 *
 * @param message_nq    The message_nq to be encoded.
 * @param dest          The destination where to write the encoded message.
 *
 * @return              Returns the number of bytes wrote on success,
 *                      -1 otherwise.
 */
extern int gnl_message_nq_to_string(struct gnl_message_nq *message_nq, char **dest);

/**
 * Decode the given message and fill the message_nq with it.
 *
 * @param message       The message to decode.
 * @param message_nq    The struct to fill with the decoded message, it must be previously
 *                      initialized with gnl_message_nq_init.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_nq_from_string(const char *message, struct gnl_message_nq *message_nq);

/**
 * Enqueue the given gnl_message_snb element into the given message_nq.
 *
 * @param message_nq    The message_nq where to enqueue the gnl_message_snb element.
 * @param el            The gnl_message_snb element.
 *
 * @return int          Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_nq_enqueue(struct gnl_message_nq *message_nq, struct gnl_message_snb *message_snb);

/**
 * Delete and return a gnl_message_snb element from the given message_nq.
 *
 * @param message_nq    The message_nq from where to dequeue an gnl_message_snb element.
 *
 * @return void*        Returns the dequeued gnl_message_snb element on success, if the
 *                      queue is empty returns NULL.
 */
extern struct gnl_message_snb *gnl_message_nq_dequeue(struct gnl_message_nq *message_nq);

#endif //GNL_MESSAGE_NQ_H