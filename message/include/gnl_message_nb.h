
#ifndef GNL_MESSAGE_NB_H
#define GNL_MESSAGE_NB_H

/**
 * The message_nb message.
 */
struct gnl_message_nb {
    int number;
    void *bytes;
};

/**
 * Create a new message_nb.
 *
 * @return The message_nb struct created.
 */
extern struct gnl_message_nb *gnl_message_nb_init();

/**
 * Create a new message_nb with the given arguments.
 *
 * @param number    The number to put into the message_nb.
 * @param bytes     The bytes of the message_nb.
 *
 * @return          The message_nb struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_nb *gnl_message_nb_init_with_args(int number, void *bytes);

/**
 * Destroy the given message_nb.
 *
 * @param message_nb    The message_nb to be destroyed.
 */
extern void gnl_message_nb_destroy(struct gnl_message_nb *message_nb);

/**
 * Encode the message_nb and put it into "dest".
 *
 * @param message_nb    The message_nb to be encoded.
 * @param dest          The destination where to write the encoded message.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_nb_write(struct gnl_message_nb message_nb, char **dest);

/**
 * Decode the given message and fill the message_nb with it.
 *
 * @param message       The message to decode.
 * @param message_nb    The struct to fill with the decoded message, it must be previously
 *                      initialized with gnl_message_nb_init.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_nb_read(const char *message, struct gnl_message_nb *message_nb);

#endif //GNL_MESSAGE_NB_H