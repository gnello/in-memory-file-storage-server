
int gnl_socket_service_connect_result;

void mock_gnl_socket_service_set_connect_result(int result) {
    gnl_socket_service_connect_result = result;
}

int gnl_socket_service_connect(const char *socket_name) {
    return gnl_socket_service_connect_result;
}