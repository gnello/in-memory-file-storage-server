#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/gnl_message_snb.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * Calculate the size of the message_snb.
 *
 * @param message_snb   The message_snb.
 *
 * @return              The size of the message_snb.
 */
static int gnl_message_snb_size(const struct gnl_message_snb *message_snb) {
    return MAX_DIGITS_INT + strlen(message_snb->string) + MAX_DIGITS_INT + message_snb->count + 1; // count also the '\0' char
}

/**
 * {@inheritDoc}
 */
struct gnl_message_snb *gnl_message_snb_init() {
    struct gnl_message_snb *message_snb = (struct gnl_message_snb *)calloc(1, sizeof(struct gnl_message_snb));
    GNL_NULL_CHECK(message_snb, ENOMEM, NULL)

    return message_snb;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_snb *gnl_message_snb_init_with_args(const char *string, size_t count, const void *bytes) {
    // validate parameters
    GNL_NULL_CHECK(string, EINVAL, NULL)

    struct gnl_message_snb *message_snb = gnl_message_snb_init();
    GNL_NULL_CHECK(message_snb, ENOMEM, NULL)

    // assign string
    message_snb->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(message_snb->string, ENOMEM, NULL)

    strncpy(message_snb->string, string, strlen(string) + 1);

    // assign the count
    message_snb->count = count;

    // assign bytes
    message_snb->bytes = calloc(count, 1);
    GNL_NULL_CHECK(message_snb->bytes, ENOMEM, NULL)

    memcpy(message_snb->bytes, bytes, count);

    return message_snb;
}

/**
 * {@inheritDoc}
 */
void gnl_message_snb_destroy(struct gnl_message_snb *message_snb) {
    if (message_snb != NULL) {
        free(message_snb->string);
        free(message_snb->bytes);
        free(message_snb);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_message_snb_to_string(const struct gnl_message_snb *message_snb, char **dest) {
    int message_snb_size = gnl_message_snb_size(message_snb);

    GNL_CALLOC(*dest, message_snb_size, -1)

    int maxlen = message_snb_size - message_snb->count;

    snprintf(*dest, maxlen, "%0*lu%s%0*lu", MAX_DIGITS_INT, strlen(message_snb->string), message_snb->string,
             MAX_DIGITS_INT, message_snb->count);

    memcpy(*dest + maxlen, message_snb->bytes, message_snb->count);

    return message_snb_size;
}

/**
 * {@inheritDoc}
 */
int gnl_message_snb_from_string(const char *message, struct gnl_message_snb *message_snb) {
    if (message_snb == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t string_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &string_len);

    // get the string
    message_snb->string = calloc(string_len + 1, sizeof(char));
    GNL_NULL_CHECK(message_snb->string, ENOMEM, -1)

    strncpy(message_snb->string, message + MAX_DIGITS_INT, string_len);

    // get the bytes length
    char bytes_len_string[MAX_DIGITS_INT];
    strncpy(bytes_len_string, message + MAX_DIGITS_INT + string_len, MAX_DIGITS_INT);

    char *ptr = NULL;
    message_snb->count = strtol(bytes_len_string, &ptr, 10);

    // if no digits found
    if (bytes_len_string == ptr) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    // get the bytes
    message_snb->bytes = calloc(message_snb->count, 1);
    GNL_NULL_CHECK(message_snb->bytes, ENOMEM, -1)

    memcpy(message_snb->bytes, message + MAX_DIGITS_INT + string_len + MAX_DIGITS_INT + 1, message_snb->count);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>