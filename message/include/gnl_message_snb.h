
#ifndef GNL_MESSAGE_SB_H
#define GNL_MESSAGE_SB_H

/**
 * The message_snb message.
 */
struct gnl_message_snb {
    char *string;
    size_t count;
    void *bytes;
};

/**
 * Create a new message_snb.
 *
 * @return The message_snb struct created.
 */
extern struct gnl_message_snb *gnl_message_snb_init();

/**
 * Create a new message_snb with the given arguments.
 *
 * @param string    The string of the message_snb.
 * @param count     The count of bytes of the message_snb bytes attribute.
 * @param bytes     The bytes of the message_snb.
 *
 * @return          The message_snb struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_snb *gnl_message_snb_init_with_args(char *string, size_t count, void *bytes);

/**
 * Destroy the given message_snb.
 *
 * @param message_snb   The message_snb to be destroyed.
 */
extern void gnl_message_snb_destroy(struct gnl_message_snb *message_snb);

/**
 * Encode the message_snb and put it into "dest".
 *
 * @param message_snb   The message_snb to be encoded.
 * @param dest          The destination where to write the encoded message.
 *
 * @return              Returns the number of bytes wrote on success,
 *                      -1 otherwise.
 */
extern int gnl_message_snb_to_string(const struct gnl_message_snb *message_snb, char **dest);

/**
 * Decode the given message and fill the message_snb with it.
 *
 * @param message       The message to decode.
 * @param message_snb   The struct to fill with the decoded message, it must be previously
 *                      initialized with gnl_message_snb_init.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_snb_from_string(const char *message, struct gnl_message_snb *message_snb);

#endif //GNL_MESSAGE_SB_H