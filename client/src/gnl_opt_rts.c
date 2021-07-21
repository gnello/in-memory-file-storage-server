#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <gnl_queue_t.h>
#include "../include/gnl_opt_rts.h"
#include <gnl_macro_beg.h>

/**
 * Recursive support function of the gnl_opt_rts_scan_dir function.
 *
 * @param dirname   The dirname where to scan the files.
 * @param queue     The queue where to put the scanned files.
 * @param count     The current number of scanned files.
 * @param limit     The max number of file to scan.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int scan_dir(const char *dirname, struct gnl_queue_t *queue, int *count, const int limit) {
    DIR *dir;
    struct dirent *entry;
    int res;

    // open the given dirname
    dir = opendir(dirname);
    GNL_NULL_CHECK(dir, errno, -1)

    // read the directory structure
    while ((entry = readdir(dir)) != NULL) {
        if (limit > 0 && *count >= limit) {
            break;
        }

        // generate the filename
        char *filename;
        unsigned long len = strlen(dirname) + 1 + strlen(entry->d_name);
        GNL_CALLOC(filename, len + 1, -1)

        snprintf(filename, len + 1, "%s/%s", dirname, entry->d_name);

        // get dirname attributes
        struct stat st;
        res = stat(filename, &st);
        GNL_MINUS1_CHECK(res, errno, -1)

        // if is a directory
        if (S_ISDIR(st.st_mode)) {
            // ignore . and ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                free(filename);

                continue;
            }

            // recursive call
            if (scan_dir(filename, queue, count, limit) == -1) {
                free(filename);
                return -1;
            }

            // free memory: filename not in a queue and no more useful
            free(filename);
        } else {
            // enqueue the file
            res = gnl_queue_enqueue(queue, filename);
            GNL_MINUS1_CHECK(res, errno, -1);

            (*count)++;
        }
    }

    // close the directory
    res = closedir(dir);
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

struct gnl_queue_t *gnl_opt_rts_scan_dir(const char *dirname, int n) {
    if (n < 0) {
        errno = EINVAL;

        return NULL;
    }

    struct gnl_queue_t *queue;
    int count = 0;

    // instantiate the queue
    queue = gnl_queue_init();
    GNL_NULL_CHECK(queue, errno, NULL);

    if (scan_dir(dirname, queue, &count, n) == -1) {
        gnl_queue_destroy(queue, NULL);

        return NULL;
    }

    return queue;
}

int gnl_opt_rts_parse_file_list(const char* file_list, struct gnl_queue_t *queue) {
    char *tok;
    char *copy_arg;
    char *token;

    GNL_CALLOC(copy_arg, strlen(file_list) + 1, -1);
    strncpy(copy_arg, file_list, strlen(file_list));

    // parse files and put it into the queue
    token = strtok_r(copy_arg, ",", &tok);
    while (token) {
        gnl_queue_enqueue(queue, token);
        token = strtok_r(NULL, ",", &tok);
    }

    free(copy_arg);

    return 0;
}

#include <gnl_macro_end.h>
