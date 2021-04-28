#include <stdio.h>
#include <stdlib.h>
#include <gnl_ts_queue_t.h>

int main() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init(GNL_TS_QUEUE_FIFO);
    //queue = gnl_ts_queue_init(GNL_TS_QUEUE_LIFO);

    if (queue == NULL) {
        perror("gnl_ts_queue_init");

        exit(EXIT_FAILURE);
    }

    char *a = "Hello";
    char *b = "World";
    char *c = "!";
    gnl_ts_queue_push(&queue, a);
    gnl_ts_queue_push(&queue, b);
    gnl_ts_queue_push(&queue, c);

    printf("%s", (char *)gnl_ts_queue_pop(&queue));
    printf("%s", (char *)gnl_ts_queue_pop(&queue));
    printf("%s", (char *)gnl_ts_queue_pop(&queue));
    printf("\n");

    gnl_ts_queue_destroy(&queue);

    return 0;
}
