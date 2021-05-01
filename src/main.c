#include <stdio.h>
#include <stdlib.h>
#include <gnl_ts_queue_t.h>
#include <gnl_ts_stack_t.h>

int main() {
    gnl_ts_queue_t *queue;
    gnl_ts_stack_t *stack;

    queue = gnl_ts_queue_init();
    stack = gnl_ts_stack_init();

    if (queue == NULL) {
        perror("gnl_ts_queue_init");

        exit(EXIT_FAILURE);
    }

    if (stack == NULL) {
        perror("gnl_ts_stack_init");

        exit(EXIT_FAILURE);
    }

    char *a = "Hello";
    char *b = "World";
    char *c = "!";

    gnl_ts_queue_enqueue(queue, a);
    gnl_ts_queue_enqueue(queue, b);
    gnl_ts_queue_enqueue(queue, c);

    gnl_ts_stack_push(stack, a);
    gnl_ts_stack_push(stack, b);
    gnl_ts_stack_push(stack, c);

    printf("Queue output: ");
    printf("%s", (char *)gnl_ts_queue_dequeue(queue));
    printf("%s", (char *)gnl_ts_queue_dequeue(queue));
    printf("%s", (char *)gnl_ts_queue_dequeue(queue));
    printf("\n");

    printf("Stack output: ");
    printf("%s", (char *)gnl_ts_stack_pop(stack));
    printf("%s", (char *)gnl_ts_stack_pop(stack));
    printf("%s", (char *)gnl_ts_stack_pop(stack));
    printf("\n");

    gnl_ts_queue_destroy(queue);

    return 0;
}
