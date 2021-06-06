#ifndef GNL_FSS_SOCKET_OPEN_H
#define GNL_FSS_SOCKET_OPEN_H

#define GNL_FSS_SOCKET_OPEN 0

/**
 * The open message.
 */
struct gnl_fss_socket_open {
    const char *pathname;
    int flags;
};

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message   The open message.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, 0 otherwise.
 */
extern int gnl_fss_socket_open_prepare_message(const struct gnl_fss_socket_open message, char **dest);

#endif //GNL_FSS_SOCKET_OPEN_H