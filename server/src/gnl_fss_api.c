
#include <linux/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <gnl_socket_request.h>
#include <gnl_socket_response.h>
#include <gnl_socket_service.h>
#include "../include/gnl_fss_api.h"
#include <gnl_macro_beg.h>

int nanosleep(const struct timespec *req, struct timespec *rem);

static struct gnl_socket_service_connection socket_service_connection;

int gnl_fss_api_open_connection(const char *sockname, int msec, const struct timespec abstime) {
    if (sockname == NULL || msec <= 0) {
        errno = EINVAL;

        return -1;
    }

    // maintain only 1 connection active to the server
    if (gnl_socket_service_is_active(&socket_service_connection)) {
        errno = EINVAL;

        return -1;
    }

    // try to connect to the given socket name
    struct gnl_socket_service_connection *tmp;
    while ((tmp = gnl_socket_service_connect(sockname)) == NULL) {
        if (errno != ENOENT) {
            return -1;
        }

        time_t now = time(NULL);

        // max wait time reached, return
        if (now > abstime.tv_sec) {
            errno = EAGAIN;

            return -1;
        }

        // wait the given amount of time
        struct timespec tim;
        tim.tv_sec = 0;
        tim.tv_nsec = 1000 * msec;

        nanosleep(&tim, NULL);
    }

    socket_service_connection = *tmp;
    free(tmp);

    return 0;
}

int gnl_fss_api_close_connection(const char *sockname) {
    if (!gnl_socket_service_is_active(&socket_service_connection)
    || socket_service_connection.socket_name == NULL
    || strcmp(socket_service_connection.socket_name, sockname) != 0) {
        errno = EINVAL;

        return -1;
    }

    int res = gnl_socket_service_close(&socket_service_connection);
    if (res == 0) {
        free(socket_service_connection.socket_name);
        socket_service_connection.socket_name = NULL;
    }

    return res;
}

int gnl_fss_api_open_file(const char *pathname, int flags) {
    if (pathname == NULL) {
        errno = EINVAL;

        return -1;
    }

    // create request
    struct gnl_socket_request *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 2, pathname, flags);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    char *message = NULL;

    int res = gnl_socket_request_write(request, &message);
    GNL_MINUS1_CHECK(res, EINVAL, -1)

    // send the request
    res = gnl_socket_service_emit(&socket_service_connection, message);
    GNL_MINUS1_CHECK(res, errno, -1)

    // clean memory
    gnl_socket_request_destroy(request);
    free(message);

    // wait the response TODO: mettere size corretta
    GNL_CALLOC(message, 10, -1)

    res = gnl_socket_service_read(&socket_service_connection, &message, 10);
    GNL_MINUS1_CHECK(res, errno, -1)

    // get the response
    struct gnl_socket_response *response = gnl_socket_response_read(message);

    // check for errors
    if (response->type == GNL_SOCKET_RESPONSE_ERROR) {
//        switch (response->payload.error->number) {
//
//        }

        return -1;
    }

    // Check for evicted files
//    if (response->payload.open->number > 0) {
//        // handle evicted files
//    }

    return 0;
}

int gnl_fss_api_read_file(const char *pathname, void **buf, size_t *size) {
    return 0;
}

int gnl_fss_api_read_N_files(int N, const char *dirname) {
    return 0;
}

int gnl_fss_api_write_file(const char *pathname, const char *dirname) {
    return 0;
}

int gnl_fss_api_append_to_file(const char *pathname, void *buf, size_t size, const char *dirname) {
    return 0;
}

int gnl_fss_api_lock_file(const char *pathname) {
    return 0;
}

int gnl_fss_api_unlock_file(const char *pathname) {
    return 0;
}

int gnl_fss_api_close_file(const char *pathname) {
    return 0;
}

int gnl_fss_api_remove_file(const char *pathname) {
    return 0;
}

#include <gnl_macro_end.h>