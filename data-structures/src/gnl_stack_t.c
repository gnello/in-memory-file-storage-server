/*
 * This is a simple stack implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/gnl_stack_t.h"
#include <gnl_macro_beg.h>

// the node of the stack
struct gnl_stack_node {
    void *data;
    struct gnl_stack_node *next;
};

/**
 * top  The pointer to the first element of the stack.
 * size The size of the stack.
 */
struct gnl_stack_t {
    struct gnl_stack_node *top;
    unsigned long size;
};

/**
 * {@inheritDoc}
 */
struct gnl_stack_t *gnl_stack_init() {
    struct gnl_stack_t *stack = (struct gnl_stack_t *)malloc(sizeof(struct gnl_stack_t));

    GNL_NULL_CHECK(stack, ENOMEM, NULL)

    // init the stack implementation data
    stack->top = NULL;
    stack->size = 0;

    return stack;
}

/**
 * {@inheritDoc}
 */
void gnl_stack_destroy(struct gnl_stack_t *s, void (*destroy)(void *data)) {
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

/**
 * {@inheritDoc}
 */
int gnl_stack_push(struct gnl_stack_t *s, void *el) {
    GNL_NULL_CHECK(s, EINVAL, -1)

    // create the new stack node
    struct gnl_stack_node *temp = (struct gnl_stack_node *)malloc(sizeof(struct gnl_stack_node));

    GNL_NULL_CHECK(temp, ENOMEM, -1)

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

/**
 * {@inheritDoc}
 */
void *gnl_stack_pop(struct gnl_stack_t *s) {
    GNL_NULL_CHECK(s, EINVAL, NULL)

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

/**
 * {@inheritDoc}
 */
unsigned long gnl_stack_size(const struct gnl_stack_t *s) {
    GNL_NULL_CHECK(s, EINVAL, -1)

    return s->size;
}

#include <gnl_macro_end.h>