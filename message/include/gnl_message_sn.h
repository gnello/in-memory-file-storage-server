
#ifndef GNL_MESSAGE_SN_H
#define GNL_MESSAGE_SN_H

/**
 * The message_sn message.
 */
struct gnl_message_sn {
    char *string;
    int number;
};

/**
 * Create a new message_sn.
 *
 * @return The message_sn struct created.
 */
extern struct gnl_message_sn *gnl_message_sn_init();

/**
 * Create a new message_sn with the given arguments.
 *
 * @param string    The string of the message_sn.
 * @param number    The number of the message_sn.
 *
 * @return          The message_sn struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_sn *gnl_message_sn_init_with_args(char *string, int number);

/**
 * Destroy the given message_sn.
 *
 * @param message_sn    The message_sn to be destroyed.
 */
extern void gnl_message_sn_destroy(struct gnl_message_sn *message_sn);

/**
 * Encode the message_sn and put it into "dest".
 *
 * @param message_sn    The message_sn to be encoded.
 * @param dest          The destination where to write the encoded message.
 *
 * @return              Returns the number of bytes wrote on success,
 *                      -1 otherwise.
 */
extern int gnl_message_sn_to_string(const struct gnl_message_sn *message_sn, char **dest);

/**
 * Decode the given message and fill the message_sn with it.
 *
 * @param message       The message to decode.
 * @param message_sn    The struct to fill with the decoded message, it must be previously
 *                      initialized with gnl_message_sn_init.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_sn_from_string(const char *message, struct gnl_message_sn *message_sn);

#endif //GNL_MESSAGE_SN_H