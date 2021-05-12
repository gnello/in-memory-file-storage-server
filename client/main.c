#include "./src/gnl_opt_handler.c"

int main(int argc, char * argv[]) {
    gnl_opt_handler *res;
    res = gnl_opt_handler_init(argc, argv);

    if (res == NULL) {
        perror("gnl_opt_handler_init");
        exit(EXIT_FAILURE);
    }

    printQueue(res);

    return 0;
}