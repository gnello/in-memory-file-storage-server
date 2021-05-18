#include <string.h>

const int GNL_FSS_SOCKET_OPEN = 1;

struct gnl_fss_socket_open {
    const char *pathname;
    int flags;
};

int gnl_fss_socket_open_size(struct gnl_fss_socket_open open) {
    return strlen(open.pathname) + sizeof(open.flags);
}