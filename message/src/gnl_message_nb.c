#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/gnl_message_nb.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * Calculate the size of the message_nb.
 *
 * @param message_nb    The message_nb.
 *
 * @return              The size of the message_nb.
 */
static int gnl_message_nb_size(const struct gnl_message_nb message_nb) {
    return MAX_DIGITS_INT + MAX_DIGITS_INT + strlen(message_nb.bytes);
}

/**
 * {@inheritDoc}
 */
struct gnl_message_nb *gnl_message_nb_init() {
    struct gnl_message_nb *message_nb = (struct gnl_message_nb *)calloc(1, sizeof(struct gnl_message_nb));
    GNL_NULL_CHECK(message_nb, ENOMEM, NULL)

    return message_nb;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_nb *gnl_message_nb_init_with_args(int number, void *bytes) {
    struct gnl_message_nb *message_nb = gnl_message_nb_init();
    GNL_NULL_CHECK(message_nb, ENOMEM, NULL)

    // assign the number
    message_nb->number = number;

    // assign bytes
    message_nb->bytes = calloc(strlen(bytes), sizeof(void *));
    GNL_NULL_CHECK(message_nb->bytes, ENOMEM, NULL)

    memcpy(message_nb->bytes, bytes, strlen(bytes));

    return message_nb;
}

/**
 * {@inheritDoc}
 */
void gnl_message_nb_destroy(struct gnl_message_nb *message_nb) {
    if (message_nb != NULL) {
        free(message_nb->bytes);
        free(message_nb);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_message_nb_write(const struct gnl_message_nb message_nb, char **dest) {
    int message_nb_size = gnl_message_nb_size(message_nb);

    GNL_CALLOC(*dest, message_nb_size + 1, -1)

    int maxlen = (message_nb_size - strlen(message_nb.bytes)) + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d%0*lu", MAX_DIGITS_INT, message_nb.number, MAX_DIGITS_INT, strlen(message_nb.bytes));

    memcpy(*dest + MAX_DIGITS_INT + MAX_DIGITS_INT, message_nb.bytes, strlen(message_nb.bytes));

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_message_nb_read(const char *message, struct gnl_message_nb *message_nb) {
    if (message_nb == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the number
    sscanf(message, "%"MAX_DIGITS_CHAR"d", &message_nb->number);

    // get the bytes length
    char bytes_len_string[MAX_DIGITS_INT];
    strncpy(bytes_len_string, message + MAX_DIGITS_INT, MAX_DIGITS_INT);

    int bytes_len;
    char *ptr = NULL;
    bytes_len = strtol(bytes_len_string, &ptr, 10);

    // if no digits found
    if (bytes_len_string == ptr) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    // get the bytes
    message_nb->bytes = calloc(bytes_len, sizeof(void *));
    GNL_NULL_CHECK(message_nb->bytes, ENOMEM, -1)

    memcpy(message_nb->bytes, message + MAX_DIGITS_INT + MAX_DIGITS_INT, bytes_len);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>