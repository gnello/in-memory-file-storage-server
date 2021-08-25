
#ifndef GNL_MESSAGE_SB_H
#define GNL_MESSAGE_SB_H

/**
 * The message_sb message.
 */
struct gnl_message_sb {
    char *string;
    void *bytes;
};

/**
 * Create a new message_sb.
 *
 * @return The message_sb struct created.
 */
extern struct gnl_message_sb *gnl_message_sb_init();

/**
 * Create a new message_sb with the given arguments.
 *
 * @param string    The string of the message_sb.
 * @param bytes     The bytes of the message_sb.
 *
 * @return          The message_sb struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_sb *gnl_message_sb_init_with_args(char *string, void *bytes);

/**
 * Destroy the given message_sb.
 *
 * @param message_sb    The message_sb to be destroyed.
 */
extern void gnl_message_sb_destroy(struct gnl_message_sb *message_sb);

/**
 * Encode the message_sb and put it into "dest".
 *
 * @param message_sb    The message_sb to be encoded.
 * @param dest          The destination where to write the encoded message.
 *
 * @return              Returns the number of bytes wrote on success,
 *                      -1 otherwise.
 */
extern int gnl_message_sb_write(struct gnl_message_sb *message_sb, char **dest);

/**
 * Decode the given message and fill the message_sb with it.
 *
 * @param message       The message to decode.
 * @param message_sb    The struct to fill with the decoded message, it must be previously
 *                      initialized with gnl_message_sb_init.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_sb_read(const char *message, struct gnl_message_sb *message_sb);

#endif //GNL_MESSAGE_SB_H