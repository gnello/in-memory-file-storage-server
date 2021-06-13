#include <stdio.h>
#include <gnl_colorshell.h>
#include "../src/gnl_fss_api.c"
#include "./mocks/gnl_fss_socket_service.c"

int main() {
    gnl_printf_yellow("> gnl_fss_api test:\n\n");

    mock_gnl_fss_socket_service_set_connect_result(1);

    gnl_fss_socket_service_connect("test_socket");
    gnl_fss_socket_service_connect("test_socket");

    printf("\n");
}