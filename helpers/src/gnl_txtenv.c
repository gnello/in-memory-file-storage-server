/*
 * This define allows the use of strtok_r and setenv functions.
 */
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/gnl_txtenv.h"

#define BUFFER_SIZE 128

/**
 * Check whether the given line is a comment or an empty row.
 *
 * @param string    The line to validate.
 *
 * @return          Returns 1 if the line starts with # or it is
 *                  an empty row, 0 otherwise. Every whitespace
 *                  before a # is considered as if it starts with a #.
 */
static int ignore_line(char *line) {
    if (line[0] == '#') {
        return 1;
    }

    int i=0;
    while (isspace(line[i])) {
        if (i == (strlen(line)-1)) {
            return 1;
        }

        if (line[i++] == '#') {
            return 1;
        }
    }

    return 0;
}

/**
 * Parse the given file and set the env vars.
 *
 * @param file      The file to be parsed.
 * @param overwrite If !=0 overwrite an existing env variable.
 *
 * @return          Returns 0 on success, -1 on failure.
 */
static int setenv_from_file(FILE *file, int overwrite) {
    char *key;
    char *value;
    char *tok;
    char buffer[BUFFER_SIZE];

    // parse all the lines
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        // check wether the line is a comment
        if (!ignore_line(buffer)) {
            // get the var
            key = strtok_r(buffer, "=", &tok);
            value = strtok_r(NULL, "\n", &tok);

            // set the var into the system environment
            if ((setenv(key, value, overwrite) != 0)) {
                perror("setenv");

                return -1;
            }
        }
    }

    if (ferror(file) != 0) {
        perror("fgets");

        return -1;
    }

    return 0;
}

int gnl_txtenv_load(const char * path, int overwrite) {
    FILE *file;
    int res = 0;

    // open the given env file
    file = fopen(path, "r");
    if (!file) {
        perror("fopen");

        return -1;
    }

    // parse the given env file
    if (setenv_from_file(file, overwrite) != 0) {
        res = 1;
    }

    fclose(file);

    return res;
}

#undef BUFFER_SIZE