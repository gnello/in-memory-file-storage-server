
#ifndef GNL_MESSAGE_N_H
#define GNL_MESSAGE_N_H

/**
 * The message_n.
 */
struct gnl_message_n {
    int number;
};

/**
 * Create a new message_n.
 *
 * @return The message_n struct created.
 */
extern struct gnl_message_n *gnl_message_n_init();

/**
 * Create a new message_n with the given arguments.
 *
 * @param number    The number to put into the message_n.
 *
 * @return          The message_n struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_n *gnl_message_n_init_with_args(int number);

/**
 * Destroy the given message_n.
 *
 * @param message_n The message_n to be destroyed.
 */
extern void gnl_message_n_destroy(struct gnl_message_n *message_n);

/**
 * Encode the message_n and put it into "dest".
 *
 * @param message_n The message_n to be encoded.
 * @param dest      The destination where to write the encoded message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_n_write(const struct gnl_message_n message_n, char **dest);

/**
 * Decode the given message and fill the message_n with it.
 *
 * @param message   The message to decode.
 * @param message_n The struct to fill with the decoded message, it must be previously
 *                  initialized with gnl_message_n_init.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_n_read(const char *message, struct gnl_message_n *message_n);

#endif //GNL_MESSAGE_N_H