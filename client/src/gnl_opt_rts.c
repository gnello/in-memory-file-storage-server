#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <gnl_queue_t.h>
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

/**
 * Recursively scan the given dirname and put n files in
 * the queue. If n=0 put all the found files into the queue.
 *
 * @param dirname   The root dirname to scan.
 * @param n         If n=0, put all files into the queue, otherwise
 *                  put into it only n files.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
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

#include <gnl_macro_end.h>
