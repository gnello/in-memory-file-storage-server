#include <stdio.h>
#include <stdlib.h>
#include <gnl_ts_queue_t.h>

int main() {
    gnl_ts_queue_t *queue;

    queue = gnl_ts_queue_init();

    if (queue == NULL) {
        perror("gnl_ts_queue_init");

        exit(EXIT_FAILURE);
    }

    char *a = "Hello";
    char *b = "World";
    char *c = "!";
    gnl_ts_queue_enqueue(queue, a);
    gnl_ts_queue_enqueue(queue, b);
    gnl_ts_queue_enqueue(queue, c);

    printf("%s", (char *)gnl_ts_queue_dequeue(queue));
    printf("%s", (char *)gnl_ts_queue_dequeue(queue));
    printf("%s", (char *)gnl_ts_queue_dequeue(queue));
    printf("\n");

    gnl_ts_queue_destroy(queue);

    return 0;
}
