#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../../include/socket/gnl_fss_socket_open.h"

#define GNL_ALLOCATE_MESSAGE(ptr, len) {        \
    ptr = (char *)malloc(len * sizeof(char));   \
    if (ptr == NULL) {                          \
        errno = ENOMEM;                         \
                                                \
        return -1;                              \
    }                                           \
}

/**
 * Calculate the size of the open message.
 *
 * @param open  The open message.
 *
 * @return      The size of the open message.
 */
static int message_size(const struct gnl_fss_socket_open open) {
    return
    sizeof(int) // the operation id
    + sizeof(size_t) // the length of the pathname
    + strlen(open.pathname) // the pathname
    + sizeof(open.flags); // the flags
}

int gnl_fss_socket_open_prepare_message(const struct gnl_fss_socket_open message, char **dest) {
    GNL_ALLOCATE_MESSAGE(*dest, message_size(message) + 1)

    sprintf(*dest, "%d%lu%s%d", GNL_FSS_SOCKET_OPEN, strlen(message.pathname), message.pathname, message.flags);

    return 0;
}

#undef GNL_ALLOCATE_MESSAGE