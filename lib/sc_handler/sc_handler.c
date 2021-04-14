#include <stdio.h>
#include <stdlib.h>

void* sc_malloc_h(size_t size) {
    void *tmp;
    if ((tmp = malloc(size)) == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    return tmp;
}