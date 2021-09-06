#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/gnl_message_nq.h"
#include "./gnl_message_snb.c"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * Destroy a gnl_message_snb element. This method must be passed
 * only to the gnl_queue_destroy invocation.
 *
 * @param ptr   The element of the queue to destroy.
 */
static void destroy_callback(void *ptr) {
    gnl_message_snb_destroy((struct gnl_message_snb *)ptr);
}

/**
 * Calculate the size of the message_nq.
 *
 * @param message_nq    The message_nq.
 *
 * @return              The size of the message_nq.
 */
static int gnl_message_nq_size(const struct gnl_message_nq *message_nq) {
    return MAX_DIGITS_INT;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_nq *gnl_message_nq_init() {
    struct gnl_message_nq *message_nq = (struct gnl_message_nq *)calloc(1, sizeof(struct gnl_message_nq));
    GNL_NULL_CHECK(message_nq, ENOMEM, NULL)

    message_nq->number = 0;

    message_nq->queue = gnl_queue_init();
    GNL_NULL_CHECK(message_nq->queue, errno, NULL)

    return message_nq;
}

/**
 * {@inheritDoc}
 */
void gnl_message_nq_destroy(struct gnl_message_nq *message_nq) {
    if (message_nq != NULL) {
        gnl_queue_destroy(message_nq->queue, destroy_callback);
        free(message_nq);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_message_nq_to_string(struct gnl_message_nq *message_nq, char **dest) {
    // validate the parameters
    GNL_NULL_CHECK(message_nq, EINVAL, -1)
    GNL_MINUS1_CHECK(-1 * (*dest != NULL), EINVAL, -1)

    size_t original_len;

    // initialize a new queue where to insert the
    // dequeued elements
    struct gnl_queue_t *new_queue = gnl_queue_init();
    GNL_NULL_CHECK(new_queue, errno, -1)

    // the dequeued gnl_message_snb struct
    struct gnl_message_snb *message_snb = NULL;

    // tmp pointer for re-alloc
    void *tmp;

    // the tmp destination where to string a gnl_message_snb element
    char *dest_tmp = NULL;
    int res;

    // get the size
    int message_nq_size = gnl_message_nq_size(message_nq);

    int maxlen = message_nq_size + 1; // count also the '\0' char

    GNL_CALLOC(*dest, maxlen, -1)

    snprintf(*dest, maxlen, "%0*d", MAX_DIGITS_INT, message_nq->number);

    // for each gnl_message_snb struct dequeued...
    while ((message_snb = gnl_queue_dequeue(message_nq->queue)) != NULL) {

        // convert to string
        res = gnl_message_snb_to_string(message_snb, &dest_tmp);
        GNL_MINUS1_CHECK(res, errno, -1)

        original_len = message_nq_size;
        message_nq_size += res;

        // re-alloc the destination
        tmp = realloc(*dest, message_nq_size);
        GNL_NULL_CHECK(tmp, errno, -1)
        *dest = tmp;

        // copy the message into dest
        memcpy(*dest + original_len, dest_tmp, res);

        // re-enqueue the message in the new queue
        res = gnl_queue_enqueue(new_queue, message_snb);
        GNL_MINUS1_CHECK(res, errno, -1)

        // free memory
        free(dest_tmp);
        dest_tmp = NULL;
    }

    // destroy the old queue since it has no elements anymore
    gnl_queue_destroy(message_nq->queue, NULL);

    // re-assign the queue to preserve the dequeued elements
    message_nq->queue = new_queue;

    return message_nq_size;
}

/**
 * {@inheritDoc}
 */
int gnl_message_nq_from_string(const char *message, struct gnl_message_nq *message_nq) {
    // validate the parameters
    GNL_NULL_CHECK(message, EINVAL, -1)

    // get the number
    char message_NQ[MAX_DIGITS_INT];
    strncpy(message_NQ, message, MAX_DIGITS_INT);

    // reset the errno value
    errno = 0;

    char *ptr = NULL;
    message_nq->number = strtol(message_NQ, &ptr, 10);

    // if no digits found
    if (message_NQ == ptr || errno != 0) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    int offset = MAX_DIGITS_INT;

    // decode the message step by step
    int left = message_nq->number;
    while (left > 0) {
        // initialize a new struct
        struct gnl_message_snb *message_snb = gnl_message_snb_init();

        // decode the step
        int res = gnl_message_snb_from_string(message + offset, message_snb);
        GNL_MINUS1_CHECK(res, errno, -1)

        // enqueue the struct
        res = gnl_queue_enqueue(message_nq->queue, message_snb);
        GNL_MINUS1_CHECK(res, errno, -1)

        // increase the offset
        offset += gnl_message_snb_size(message_snb);

        left--;
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_message_nq_enqueue(struct gnl_message_nq *message_nq, struct gnl_message_snb *gnl_message_snb) {
    GNL_NULL_CHECK(message_nq, EINVAL, -1)
    GNL_NULL_CHECK(gnl_message_snb, EINVAL, -1)

    int res = gnl_queue_enqueue(message_nq->queue, gnl_message_snb);
    GNL_MINUS1_CHECK(res, errno, -1)

    message_nq->number++;

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_snb *gnl_message_nq_dequeue(struct gnl_message_nq *message_nq) {
    GNL_NULL_CHECK(message_nq, EINVAL, NULL)

    struct gnl_message_snb *res = gnl_queue_dequeue(message_nq->queue);
    GNL_NULL_CHECK(res, errno, NULL)

    message_nq->number--;

    return res;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>