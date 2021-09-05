#include <limits.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <gnl_queue_t.h>
#include "../include/gnl_opt_rts.h"
#include <gnl_macro_beg.h>

#define BUFFER 1000

int output = 0;

static void enable_output() {
    output = 1;
}

char *realpath(const char* restrict path, char* restrict resolved_path);

/**
 * Return the size of the file pointed by the given pathname.
 *
 * @param pathname  The absolute or relative path of the target file.
 *
 * @return          Returns the size of the file on success,
 *                  -1 on failure.
 */
off_t file_size(const char* pathname){
    struct stat st;

    int res = stat(pathname, &st);
    GNL_MINUS1_CHECK(res, errno, -1)

    return st.st_size;
}

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
    char *filename = NULL;

    // open the given dirname
    dir = opendir(dirname);
    GNL_NULL_CHECK(dir, errno, -1)

    // read the directory structure
    while ((entry = readdir(dir)) != NULL) {
        if (limit > 0 && *count >= limit) {
            break;
        }

        // generate the filename
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
        } else {
            char *buffer;
            char *res_ptr;

            // get the absolute path of the file
            GNL_CALLOC(buffer, PATH_MAX, -1);

            res_ptr = realpath(filename, buffer);
            GNL_NULL_CHECK(res_ptr, errno, -1);

            // enqueue the file
            res = gnl_queue_enqueue(queue, buffer);
            GNL_MINUS1_CHECK(res, errno, -1);

            (*count)++;
        }

        // free memory
        free(filename);
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

/**
 * {@inheritDoc}
 */
int gnl_opt_rts_parse_file_list(const char *file_list, struct gnl_queue_t *queue) {
    char *tok;
    char *copy_arg;
    char *token;
    char *buffer;
    char *res_ptr;
    int res;

    GNL_CALLOC(copy_arg, (strlen(file_list) + 1) * sizeof(char), -1);
    strncpy(copy_arg, file_list, strlen(file_list));

    // parse files and put it into the queue
    token = strtok_r(copy_arg, ",", &tok);
    while (token) {

        // get the absolute path of the file
        GNL_CALLOC(buffer, PATH_MAX, -1);

        res_ptr = realpath(token, buffer);
        GNL_NULL_CHECK(res_ptr, errno, -1);

        // enqueue the token (i.e. the filename)
        res = gnl_queue_enqueue(queue, buffer);
        GNL_MINUS1_CHECK(res, errno, -1);

        // proceed to the next token
        token = strtok_r(NULL, ",", &tok);
    }

    free(copy_arg);

    return 0;
}

/**
 * {@inheritDoc}
 */
static int filename_arg_walk(const char *arg, int (*callback)(const char *filename)) {
    int res;
    struct gnl_queue_t *queue;
    char *filename;

    // initialize the queue
    queue = gnl_queue_init();
    GNL_NULL_CHECK(queue, errno, -1);

    // parse arg
    res = gnl_opt_rts_parse_file_list(arg, queue);
    GNL_MINUS1_CHECK(res, errno, -1);

    // for each given files
    while ((filename = (char *)gnl_queue_dequeue(queue)) != NULL) {

        // apply the given function
        res = callback(filename);

        free(filename);

        // if an error happen stop the execution
        if (res == -1) {
            break;
        }
    }

    // destroy the queue
    gnl_queue_destroy(queue, free);

    return res;
}

/**
 * http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
 *
 * @param dir   The pathname to create.
 */
static void nico_mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);

    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
}

static int save_permanent(const char *filename, const char *dirname, void *buf, size_t count) {
    // validate parameters
    GNL_NULL_CHECK(filename, EINVAL, -1);
    GNL_NULL_CHECK(dirname, EINVAL, -1);

    // check if dirname exists
    DIR *dir = opendir(dirname);
    GNL_NULL_CHECK(dir, errno, -1)

    closedir(dir);

    // create the complete path
    char *complete_path;
    unsigned long len = strlen(dirname) + strlen(filename) + 1;
    GNL_CALLOC(complete_path, len, -1)

    snprintf(complete_path, len, "%s%s", dirname, filename);

    // create filename directories if necessary
    nico_mkdir(complete_path);

    // write the file
    FILE *fp;

    fp = fopen(complete_path , "w");
    GNL_NULL_CHECK(fp, errno, -1)

    unsigned long nwrite = fwrite(buf, 1, count, fp);
    GNL_MINUS1_CHECK(nwrite, errno, -1)

    fclose(fp);

    // free memory
    free(complete_path);

    return 0;
}

static void print_command(const char command, const char *args) {
    if (output == 0) {
        return;
    }

    printf("Executing command -%c %s:\n", command, args == NULL ? "" : args);
}

static void print_row(const char *op, const char *target, const char *res, const char *message, va_list a_list) {
    if (output == 0) {
        return;
    }

    char msg[BUFFER];

    // print variables into the message
    if (message != NULL) {

        vsnprintf(msg, sizeof(msg), message, a_list);
    }

    printf("%s - %s - %s - %s\n", op, target, res, message == NULL ? "" : msg);
}

static void print_log(const char *op, const char *target, int res, const char *message, ...) {
    if (output == 0) {
        return;
    }

    va_list a_list;
    va_start(a_list, message);

    print_row(op, target, res == 0 ? "OK" : "KO", res == 0 ? message : strerror(errno), a_list);

    va_end(a_list);
}

#include <gnl_macro_end.h>
