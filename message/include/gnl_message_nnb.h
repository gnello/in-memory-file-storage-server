
#ifndef GNL_MESSAGE_NNB_H
#define GNL_MESSAGE_NNB_H

/**
 * The message_nnb message.
 */
struct gnl_message_nnb {
    int number;
    size_t count;
    void *bytes;
};

/**
 * Create a new message_nnb.
 *
 * @return The message_nnb struct created.
 */
extern struct gnl_message_nnb *gnl_message_nnb_init();

/**
 * Create a new message_nnb with the given arguments.
 *
 * @param number    The number to put into the message_nnb.
 * @param count     The count of bytes of the message_nnb bytes attribute.
 * @param bytes     The bytes of the message_nnb.
 *
 * @return          The message_nnb struct created on success,
 *                  NULL otherwise.
 */
extern struct gnl_message_nnb *gnl_message_nnb_init_with_args(int number, size_t count, void *bytes);

/**
 * Destroy the given message_nnb.
 *
 * @param message_nnb    The message_nnb to be destroyed.
 */
extern void gnl_message_nnb_destroy(struct gnl_message_nnb *message_nnb);

/**
 * Encode the message_nnb and put it into "dest".
 *
 * @param message_nnb   The message_nnb to be encoded.
 * @param dest          The destination where to write the encoded message.
 *
 * @return              Returns the number of bytes wrote on success,
 *                      -1 otherwise.
 */
extern int gnl_message_nnb_to_string(const struct gnl_message_nnb *message_nnb, char **dest);

/**
 * Decode the given message and fill the message_nnb with it.
 *
 * @param message       The message to decode.
 * @param message_nnb   The struct to fill with the decoded message, it must be previously
 *                      initialized with gnl_message_nnb_init.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_message_nnb_from_string(const char *message, struct gnl_message_nnb *message_nnb);

#endif //GNL_MESSAGE_NNB_H