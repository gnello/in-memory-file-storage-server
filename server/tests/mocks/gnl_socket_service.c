#include <stdlib.h>
#include <string.h>
#include "../../socket/include/gnl_socket_service.h"
#include <gnl_macro_beg.h>

int gnl_socket_service_connect_result;
int gnl_socket_service_close_result;

void mock_gnl_socket_service_set_connect_result(int result) {
    gnl_socket_service_connect_result = result;
}

void mock_gnl_socket_service_set_close_connection_result(int result) {
    gnl_socket_service_close_result = result;
}

struct gnl_socket_service_connection *gnl_socket_service_connect(const char *socket_name) {
    if (gnl_socket_service_connect_result >= 0) {
        struct gnl_socket_service_connection *connection = (struct gnl_socket_service_connection *)calloc(1,
                sizeof(struct gnl_socket_service_connection));

        connection->fd = 0;
        connection->active = 1;

        GNL_CALLOC(connection->socket_name, strlen(socket_name) + 1, NULL)
        strcpy(connection->socket_name, socket_name);

        return connection;
    }

    return NULL;
}

int gnl_socket_service_close(struct gnl_socket_service_connection *connection) {
    if (gnl_socket_service_close_result >= 0) {
        free(connection->socket_name);
        free(connection);
    }

    return gnl_socket_service_close_result;
}

#include <gnl_macro_end.h>