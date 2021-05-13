#include "./src/gnl_opt_handler.c"

int main(int argc, char * argv[]) {
    gnl_opt_handler *handler;
    handler = gnl_opt_handler_init();

    if (handler == NULL) {
        perror("gnl_opt_handler_init");
        exit(EXIT_FAILURE);
    }

    gnl_opt_handler_parse_opt(handler, argc, argv);
    //gnl_opt_handler_handle(handler);

    printQueue(handler);

    gnl_opt_handler_destroy(handler);

    return 0;
}