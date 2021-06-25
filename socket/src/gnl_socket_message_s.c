#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The message_s.
 */
struct gnl_socket_message_s {
    char *string;
};

/**
 * Calculate the size of the message_s.
 *
 * @param message_s The message_s.
 *
 * @return          The size of the message_s.
 */
static int gnl_socket_message_s_size(const struct gnl_socket_message_s message_s) {
    return MAX_DIGITS_INT + strlen(message_s.string);
}

/**
 * Create a new message_s.
 */
struct gnl_socket_message_s *gnl_socket_message_s_init() {
    struct gnl_socket_message_s *message_s = (struct gnl_socket_message_s *)calloc(1, sizeof(struct gnl_socket_message_s));
    GNL_NULL_CHECK(message_s, ENOMEM, NULL)

    return message_s;
}

/**
 * Create a new message_s with the given arguments.
 *
 * @param string    The string of the message_s.
 */
struct gnl_socket_message_s *gnl_socket_message_s_init_with_args(char *string) {
    struct gnl_socket_message_s *message_s = gnl_socket_message_s_init();
    GNL_NULL_CHECK(message_s, ENOMEM, NULL)

    message_s->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(message_s->string, ENOMEM, NULL)

    strncpy(message_s->string, string, strlen(string) + 1);

    return message_s;
}

/**
 * Destroy the given message_s.
 *
 * @param message   The message_s to be destroyed.
 */
void gnl_socket_message_s_destroy(struct gnl_socket_message_s *message) {
    if (message != NULL) {
        free(message->string);
        free(message);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message   The message_s.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_socket_message_s_write(const struct gnl_socket_message_s message, char **dest) {
    int message_size = gnl_socket_message_s_size(message);

    GNL_CALLOC(*dest, message_size + 1, -1)

    int maxlen = message_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s", MAX_DIGITS_INT, strlen(message.string), message.string);

    return 0;
}

/**
 * Read the socket message and fill the message_s with it.
 *
 * @param message   The message_s message to read.
 * @param message_s The struct to fill with the message, it must be previously
 *                  initialized with gnl_socket_message_s_init.
 *
 * @return          Returns a pointer to the created gnl_socket_message_s
 *                  on success, NULL otherwise.
 */
int gnl_socket_message_s_read(const char *message, struct gnl_socket_message_s *message_s) {
    if (message_s == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t pathname_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &pathname_len);

    // get the string string
    message_s->string = calloc(pathname_len + 1, sizeof(char));
    GNL_NULL_CHECK(message_s->string, ENOMEM, -1)

    strncpy(message_s->string, message + MAX_DIGITS_INT, pathname_len);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>