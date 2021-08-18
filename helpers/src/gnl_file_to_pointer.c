#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
 * {@inheritDoc}
 */
int gnl_file_to_pointer(char *filepath, char **pointer, long *size) {
    int res;

    // open the file
    FILE* fp = fopen(filepath, "rb");
    if (fp == NULL) {
        return -1;
    }

    // seek at the end to get the size of the file
    res = fseek(fp, 0L, SEEK_END);
    if (res == -1) {
        fclose(fp);

        return -1;
    }

    // get the size of the file and seek at the beginning of it
    *size = ftell(fp);
    rewind(fp);

    // allocate memory
     *pointer = calloc(*size + 1, sizeof(char));
    if (*pointer == NULL) {
        fclose(fp);
        errno = ENOMEM;

        return -1;
    }

    // read the file
    if (fread(*pointer, sizeof(char), *size, fp) <= 0) {
        int errno_cpy = errno;

        if (ferror(fp)) {
            fclose(fp);
            free(*pointer);
            errno = errno_cpy;

            return -1;
        }
    }

    res = fclose(fp);
    if (res == EOF) {
        return -1;
    }

    return 0;
}