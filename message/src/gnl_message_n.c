#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/gnl_message_n.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * Calculate the size of the message_n.
 *
 * @param message_n The message_n.
 *
 * @return          The size of the message_n.
 */
static int gnl_message_n_size(const struct gnl_message_n *message_n) {
    return MAX_DIGITS_INT;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_n *gnl_message_n_init() {
    struct gnl_message_n *message_n = (struct gnl_message_n *)calloc(1, sizeof(struct gnl_message_n));
    GNL_NULL_CHECK(message_n, ENOMEM, NULL)

    return message_n;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_n *gnl_message_n_init_with_args(int number) {
    struct gnl_message_n *message_n = gnl_message_n_init();
    GNL_NULL_CHECK(message_n, ENOMEM, NULL)

    message_n->number = number;

    return message_n;
}

/**
 * {@inheritDoc}
 */
void gnl_message_n_destroy(struct gnl_message_n *message_n) {
    if (message_n != NULL) {
        free(message_n);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_message_n_to_string(const struct gnl_message_n *message_n, char **dest) {
    int message_n_size = gnl_message_n_size(message_n);

    int maxlen = message_n_size + 1; // count also the '\0' char

    GNL_CALLOC(*dest, maxlen, -1)

    snprintf(*dest, maxlen, "%0*d", MAX_DIGITS_INT, message_n->number);

    return maxlen;
}

/**
 * {@inheritDoc}
 */
int gnl_message_n_from_string(const char *message, struct gnl_message_n *message_n) {
    if (message_n == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the number
    char message_N[MAX_DIGITS_INT + 1];
    strncpy(message_N, message, MAX_DIGITS_INT);
    message_N[MAX_DIGITS_INT] = '\0';

    // reset the errno value
    errno = 0;

    char *ptr = NULL;
    message_n->number = strtol(message_N, &ptr, 10);

    // if no digits found
    if (message_N == ptr || errno != 0) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>