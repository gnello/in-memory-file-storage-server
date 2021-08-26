#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/gnl_message_sn.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * Calculate the size of the message_sn.
 *
 * @param message_sn    The message_sn.
 *
 * @return              The size of the message_sn.
 */
static int gnl_message_sn_size(const struct gnl_message_sn *message_sn) {
    return MAX_DIGITS_INT + strlen(message_sn->string) + MAX_DIGITS_INT;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_sn *gnl_message_sn_init() {
    struct gnl_message_sn *message_sn = (struct gnl_message_sn *)calloc(1, sizeof(struct gnl_message_sn));
    GNL_NULL_CHECK(message_sn, ENOMEM, NULL)

    return message_sn;
}

/**
 * {@inheritDoc}
 */
struct gnl_message_sn *gnl_message_sn_init_with_args(char *string, int number) {
    struct gnl_message_sn *message_sn = gnl_message_sn_init();
    GNL_NULL_CHECK(message_sn, ENOMEM, NULL)

    message_sn->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(message_sn->string, ENOMEM, NULL)

    strncpy(message_sn->string, string, strlen(string) + 1);
    message_sn->number = number;

    return message_sn;
}

/**
 * {@inheritDoc}
 */
void gnl_message_sn_destroy(struct gnl_message_sn *message_sn) {
    if (message_sn != NULL) {
        free(message_sn->string);
        free(message_sn);
    }
}

/**
 * {@inheritDoc}
 */
int gnl_message_sn_to_string(const struct gnl_message_sn *message_sn, char **dest) {
    int message_sn_size = gnl_message_sn_size(message_sn);

    int maxlen = message_sn_size + 1; // count also the '\0' char

    GNL_CALLOC(*dest, maxlen, -1)

    snprintf(*dest, maxlen, "%0*lu%s%0*d", MAX_DIGITS_INT, strlen(message_sn->string), message_sn->string,
             MAX_DIGITS_INT, message_sn->number);

    return maxlen;
}

/**
 * {@inheritDoc}
 */
int gnl_message_sn_from_string(const char *message, struct gnl_message_sn *message_sn) {
    if (message_sn == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t string_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &string_len);

    // get the string
    message_sn->string = calloc(string_len + 1, sizeof(char));
    GNL_NULL_CHECK(message_sn->string, ENOMEM, -1)

    strncpy(message_sn->string, message + MAX_DIGITS_INT, string_len);

    // get the number
    char message_N[MAX_DIGITS_INT];
    strncpy(message_N, message + MAX_DIGITS_INT + string_len, MAX_DIGITS_INT);

    // reset the errno value
    errno = 0;

    char *ptr = NULL;
    message_sn->number = strtol(message_N, &ptr, 10);

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