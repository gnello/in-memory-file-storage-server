/*
 * This is a simple stack implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/gnl_stack_t.h"

// the node of the stack
struct gnl_stack_node {
    void *data;
    struct gnl_stack_node *next;
};

// the stack
struct gnl_stack_t {
    struct gnl_stack_node *top;
    unsigned long size;
};

gnl_stack_t *gnl_stack_init() {
    gnl_stack_t *stack = (struct gnl_stack_t *)malloc(sizeof(struct gnl_stack_t));

    if (stack == NULL) {
        perror("malloc");

        return NULL;
    }

    // init the stack implementation data
    stack->top = NULL;
    stack->size = 0;

    return stack;
}

void gnl_stack_destroy(gnl_stack_t *s, void (*destroy)(void *data)) {
    if (s != NULL) {
        // destroy every node
        while(s->top != NULL) {
            struct gnl_stack_node *temp = s->top;
            s->top = s->top->next;

            if (destroy != NULL) {
                destroy(temp->data);
            }

            free(temp);
        }

        free(s);
    }
}

int gnl_stack_push(gnl_stack_t *s, void *el) {
    // create the new stack node
    struct gnl_stack_node *temp = (struct gnl_stack_node *)malloc(sizeof(struct gnl_stack_node));

    if (temp == NULL) {
        perror("malloc");

        return -1;
    }

    temp->data = el;
    temp->next = NULL;

    // put the element into the top of the stack
    if (s->top != NULL) {
        temp->next = s->top;
    }

    s->top = temp;
    s->size++;

    return 0;
}

void *gnl_stack_pop(gnl_stack_t *s) {
    // if stack is empty return NULL.
    if (s->top == NULL) {
        return NULL;
    }

    // move top to the next node
    struct gnl_stack_node *temp = s->top;

    s->top = s->top->next;
    s->size--;

    void *data = temp->data;

    free(temp);

    return data;
}

unsigned long gnl_stack_size(const gnl_stack_t *s) {
    return s->size;
}