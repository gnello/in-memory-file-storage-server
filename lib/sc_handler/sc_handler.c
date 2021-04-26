#include <stdio.h>
#include <stdlib.h>

/**
 * Performs a malloc, exit the program on failure.
 *
 * @param size  The size to allocate.
 * @return void*    The pointer to the memory allocated.
 */
void* sc_malloc_h(size_t size) {
    void *tmp;

    if ((tmp = malloc(size)) == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    return tmp;
}

// gcc -std=c99 -Wall sc_handler.c -c -fPIC -o sc_handler.o
// gcc -shared -o sc_handler.so sc_handler.o