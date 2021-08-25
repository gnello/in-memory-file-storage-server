
#ifndef GNL_MESSAGE_S_H
#define GNL_MESSAGE_S_H

/**
 * The message_s.
 */
struct gnl_message_s {
    char *string;
};

/**
 * Create a new message_s.
 */
extern struct gnl_message_s *gnl_message_s_init();

/**
 * Create a new message_s with the given arguments.
 *
 * @param string    The string of the message_s.
 *
 * @return          The message_s struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_s *gnl_message_s_init_with_args(char *string);

/**
 * Destroy the given message_s.
 *
 * @param message   The message_s to be destroyed.
 */
extern void gnl_message_s_destroy(struct gnl_message_s *message);

/**
 * Encode the message_s and put it into "dest".
 *
 * @param message   The message_s to be encoded.
 * @param dest      The destination where to write the encoded message.
 *
 * @return          Returns the number of bytes wrote on success,
 *                  -1 otherwise.
 */
extern int gnl_message_s_write(const struct gnl_message_s *message, char **dest);

/**
 * Decode the given message and fill the message_s with it.
 *
 * @param message   The message to decode.
 * @param message_s The struct to fill with the decoded message, it must be previously
 *                  initialized with gnl_message_s_init.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_s_read(const char *message, struct gnl_message_s *message_s);

#endif //GNL_MESSAGE_S_H