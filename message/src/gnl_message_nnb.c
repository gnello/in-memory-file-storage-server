#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/gnl_message_nnb.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * Calculate the size of the message_nnb.
 *
 * @param message_nnb   The message_nnb.
 *
 * @return              The size of the message_nnb.
 */
static int gnl_message_nnb_size(const struct gnl_message_nnb *message_nnb) {
    return MAX_DIGITS_INT + MAX_DIGITS_INT + message_nnb->count;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_nnb *gnl_message_nnb_init() {
    struct gnl_message_nnb *message_nnb = (struct gnl_message_nnb *)calloc(1, sizeof(struct gnl_message_nnb));
    GNL_NULL_CHECK(message_nnb, ENOMEM, NULL)

    return message_nnb;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_nnb *gnl_message_nnb_init_with_args(int number, size_t count, void *bytes) {
    struct gnl_message_nnb *message_nnb = gnl_message_nnb_init();
    GNL_NULL_CHECK(message_nnb, ENOMEM, NULL)

    // assign the number
    message_nnb->number = number;

    // assign the count
    message_nnb->count = count;

    // assign bytes
    message_nnb->bytes = calloc(count, sizeof(void *));
    GNL_NULL_CHECK(message_nnb->bytes, ENOMEM, NULL)

    memcpy(message_nnb->bytes, bytes, count);

    return message_nnb;
}

/**
 * {@inheritDoc}
 */
void gnl_message_nnb_destroy(struct gnl_message_nnb *message_nnb) {
    if (message_nnb != NULL) {
        free(message_nnb->bytes);
        free(message_nnb);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_message_nnb_to_string(const struct gnl_message_nnb *message_nnb, char **dest) {
    int message_nnb_size = gnl_message_nnb_size(message_nnb);

    GNL_CALLOC(*dest, message_nnb_size + 1, -1)

    int maxlen = (message_nnb_size - message_nnb->count) + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d%0*lu", MAX_DIGITS_INT, message_nnb->number, MAX_DIGITS_INT, message_nnb->count);

    memcpy(*dest + maxlen, message_nnb->bytes, message_nnb->count);

    return message_nnb_size + 1;
}

/**
 * {@inheritDoc}
 */
int gnl_message_nnb_from_string(const char *message, struct gnl_message_nnb *message_nnb) {
    if (message_nnb == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the number and the count of bytes
    sscanf(message, "%"MAX_DIGITS_CHAR"d%"MAX_DIGITS_CHAR"lu", &message_nnb->number, &message_nnb->count);

    // get the bytes
    message_nnb->bytes = calloc(message_nnb->count, sizeof(void *));
    GNL_NULL_CHECK(message_nnb->bytes, ENOMEM, -1)

    memcpy(message_nnb->bytes, message + MAX_DIGITS_INT + MAX_DIGITS_INT, message_nnb->count);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>