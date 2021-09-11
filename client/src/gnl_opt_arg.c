#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gnl_fss_api.h>
#include <gnl_queue_t.h>
#include <gnl_file_saver.h>
#include <gnl_print_table.h>
#include "./gnl_opt_rts.c"
#include "../include/gnl_opt_arg.h"
#include <gnl_macro_beg.h>

#define SOCKET_ATTEMPTS_INTERVAL 1000
#define SOCKET_WAIT_SEC 5

/**
 * Send the given file to the server using APIs.
 *
 * @param filename      The file to send to the server.
 * @param store_dirname The path where to store the trashed files from
 *                      the server.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_opt_arg_send_file(const char *filename, const char *store_dirname) {
    int res;

    // wait if we have to
    wait_milliseconds();

    // create and open the file on the server (with lock)
    res = gnl_fss_api_open_file(filename, O_CREATE | O_LOCK);

    print_log("Open file with O_CREATE|O_LOCK flags", filename, res, NULL);

    GNL_MINUS1_CHECK(res, errno, -1);

    // wait if we have to
    wait_milliseconds();

    // send the file to the server
    int res_write = gnl_fss_api_write_file(filename, store_dirname);
    int errno_write = errno;

    //get the filename size
    off_t size = file_size(filename);

    print_log("Write file", filename, res_write, "%d bytes written", size);

    // an eventual error during the writing will be checked later

    // wait if we have to
    wait_milliseconds();

    // unlock the file
    int res_unlock = gnl_fss_api_unlock_file(filename);
    int errno_unlock = errno;

    print_log("Unlock file", filename, res_unlock, NULL);

    // an eventual error during the unlocking will be checked later

    // wait if we have to
    wait_milliseconds();

    // close the file
    int res_close = gnl_fss_api_close_file(filename);
    int errno_close = errno;

    print_log("Close file", filename, res_close, NULL);

    // check if there was an error during the writing
    GNL_MINUS1_CHECK(res_write, errno_write, -1);

    // check if there was an error during the unlocking
    GNL_MINUS1_CHECK(res_unlock, errno_unlock, -1);

    // check if there was an error during the close_file
    GNL_MINUS1_CHECK(res_close, errno_close, -1);

    return 0;
}

/**
 * Remove the given file from the server using APIs.
 *
 * @param filename  The file to remove from the server.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int gnl_opt_arg_remove_file(const char *filename) {
    int res;

    // wait if we have to
    wait_milliseconds();

    // open the file on the server (with lock)
    res = gnl_fss_api_open_file(filename, O_LOCK);

    print_log("Open file with O_LOCK flags", filename, res, NULL);

    GNL_MINUS1_CHECK(res, errno, -1);

    // wait if we have to
    wait_milliseconds();

    // remove the file from the server
    int res_remove = gnl_fss_api_remove_file(filename);
    int errno_remove = errno;

    print_log("Remove file", filename, res_remove, NULL);

    // an eventual error during the remove will be checked later

    // wait if we have to
    wait_milliseconds();

    // close the file
    int res_close = gnl_fss_api_close_file(filename);
    int errno_close = errno;

    print_log("Close file", filename, res_close, NULL);

    // check if there was an error during the remove
    GNL_MINUS1_CHECK(res_remove, errno_remove, -1);

    // check if there was an error during the close_file
    GNL_MINUS1_CHECK(res_close, errno_close, -1);

    return 0;
}

/**
 * Lock the given file using APIs.
 *
 * @param filename  The file to lock.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int gnl_opt_arg_lock_file(const char *filename) {
    int res;

    // wait if we have to
    wait_milliseconds();

    // open the file on the server
    res = gnl_fss_api_open_file(filename, 0);

    print_log("Open file with no flags", filename, res, NULL);

    GNL_MINUS1_CHECK(res, errno, -1);

    // wait if we have to
    wait_milliseconds();

    // lock the file
    int res_lock = gnl_fss_api_lock_file(filename);
    int errno_lock = errno;

    print_log("Lock file", filename, res_lock, NULL);

    // an eventual error during the locking will be checked later

    // wait if we have to
    wait_milliseconds();

    // close the file
    int res_close = gnl_fss_api_close_file(filename);
    int errno_close = errno;

    print_log("Close file", filename, res_close, NULL);

    // check if there was an error during the locking
    GNL_MINUS1_CHECK(res_lock, errno_lock, -1);

    // check if there was an error during the close_file
    GNL_MINUS1_CHECK(res_close, errno_close, -1);

    return 0;
}

/**
 * Unock the given file using APIs.
 *
 * @param filename  The file to unlock.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int gnl_opt_arg_unlock_file(const char *filename) {
    int res;

    // wait if we have to
    wait_milliseconds();

    // open the file on the server
    res = gnl_fss_api_open_file(filename, 0);

    print_log("Open file with no flags", filename, res, NULL);

    GNL_MINUS1_CHECK(res, errno, -1);

    // wait if we have to
    wait_milliseconds();

    // unlock the file
    int res_unlock = gnl_fss_api_unlock_file(filename);
    int errno_unlock = errno;

    print_log("Unlock file", filename, res_unlock, NULL);

    // an eventual error during the unlocking will be checked later

    // wait if we have to
    wait_milliseconds();

    // close the file
    int res_close = gnl_fss_api_close_file(filename);
    int errno_close = errno;

    print_log("Close file", filename, res_close, NULL);

    // check if there was an error during the unlocking
    GNL_MINUS1_CHECK(res_unlock, errno_unlock, -1);

    // check if there was an error during the close_file
    GNL_MINUS1_CHECK(res_close, errno_close, -1);

    return 0;
}

/**
 * Read the given filename from the server using APIs.
 *
 * @param filename      The filename to read.
 * @param store_dirname The dirname where to store the file read.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_opt_arg_read_file(const char *filename, const char *store_dirname) {
    int res;

    // wait if we have to
    wait_milliseconds();

    // open the file on the server
    res = gnl_fss_api_open_file(filename, 0);

    print_log("Open file with no flags", filename, res, NULL);

    GNL_MINUS1_CHECK(res, errno, -1);

    // read the file
    void *buf = NULL;
    size_t size;

    // wait if we have to
    wait_milliseconds();

    int res_read = gnl_fss_api_read_file(filename, &buf, &size);
    int errno_read = errno;

    print_log("Read file", filename, res_read, "%d bytes read", size);

    // an eventual error during the read will be checked later

    // wait if we have to
    wait_milliseconds();

    // close the file
    int res_close = gnl_fss_api_close_file(filename);
    int errno_close = errno;

    print_log("Close file", filename, res_close, NULL);

    // check first if there was an error during the read
    GNL_MINUS1_CHECK(res_read, errno_read, -1);

    // store the read file on disk
    if (store_dirname != NULL) {
        res = gnl_file_saver_save(filename, store_dirname, buf, size);
    }

    //free memory
    free(buf);

    // check first if there was an error during the save
    GNL_MINUS1_CHECK(res, errno, -1);

    // check if there was an error during the close_file
    GNL_MINUS1_CHECK(res_close, errno_close, -1);

    return 0;
}

/**
 * {@inheritDoc}
 */
void arg_h(const char* program_name) { //7
    printf("Usage: %s [options]\n", program_name);
    printf("Write and read files to and from the In Memory Storage Server.\n");
    printf("Example: %s -f /tmp/fss.sk -r file1 -d /dev/null -w ./mydir\n", program_name);
    printf("\n");
    printf("The order of the options matters, the options will be handled in the order\n");
    printf("they appear.\n");
    printf("The -f, -h, -p options can not be specified more than once.\n");
    printf("The -f option must be always specified.\n");
    printf("\n");

    gnl_print_table("-h", "Print this message and exit.\n");
    gnl_print_table("-f FILENAME", "Connect to the FILENAME socket.\n");

    gnl_print_table("-w DIRNAME[,N=0]", "Send DIRNAME files to the Server.\n");
    gnl_print_table("", "If N is specified, send N files of DIRNAME\n");
    gnl_print_table("", "to the Server.\n");

    gnl_print_table("-W FILE1[,FILE2...]", "Send any given FILE/s to the Server.\n");

    gnl_print_table("-D DIRNAME", "Store the files trashed by the Server into DIRNAME.\n");
    gnl_print_table("", "It must following a -w or -W option.\n");

    gnl_print_table("-r FILE1[,FILE2...]", "Read FILE/s from the Server.\n");

    gnl_print_table("-R [N=0]", "Read all files stored on the Server.\n");
    gnl_print_table("", "If N is specified, read N random files from\n");
    gnl_print_table("", "the Server.\n");

    gnl_print_table("-d DIRNAME", "Store the files read from the Server into DIRNAME.\n");
    gnl_print_table("", "It must following a -r or -R option.\n");

    gnl_print_table("-t TIME", "Wait TIME milliseconds between sequential requests\n");
    gnl_print_table("", "to the Server.\n");

    gnl_print_table("-l FILE1[,FILE2...]", "Acquire the lock on FILE/s.\n");
    gnl_print_table("-u FILE1[,FILE2...]", "Release the lock on FILE/s.\n");

    gnl_print_table("-c FILE1[,FILE2...]", "Remove FILE/s from the Server.\n");
    gnl_print_table("-p", "Print the log of the requests made to the server.\n");
}

/**
 * {@inheritDoc}
 */
int arg_f_start(const char* socket_name) { //3
    time_t now = time(NULL);

    struct timespec tim;
    tim.tv_sec = now + SOCKET_WAIT_SEC;
    tim.tv_nsec = 0;

    print_command('f', socket_name);

    int res = gnl_fss_api_open_connection(socket_name, SOCKET_ATTEMPTS_INTERVAL, tim);

    print_log("Connect to socket", socket_name, res, NULL);

    return res;
}

/**
 * {@inheritDoc}
 */
int arg_f_end(const char* socket_name) { //3
    return gnl_fss_api_close_connection(socket_name);
}

/**
 * Parse argument of -w opt from format dirname[,n=0].
 *
 * @param arg       The argument to parse.
 * @param dirname   The destination where to write the parsed dirname.
 * @param n         The destination where to write the parsed n.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int arg_w_parse_arg(const char* arg, char **dirname, int *n) {
    char *tok;
    char *copy_arg;
    char *tmp;
    char *string_n;

    GNL_CALLOC(copy_arg, strlen(arg) + 1, -1);
    strncpy(copy_arg, arg, strlen(arg));

    // parse dirname
    tmp = strtok_r(copy_arg, ",", &tok);

    GNL_CALLOC(*dirname, strlen(tmp) + 1, -1);
    strncpy(*dirname, tmp, strlen(tmp));

    // parse n
    string_n = strtok_r(NULL, ",", &tok);

    if (string_n != NULL) {
        GNL_TO_INT(*n, string_n, -1)
    } else {
        *n = 0;
    }

    free(copy_arg);

    return 0;
}

/**
 * {@inheritDoc}
 */
int arg_w(const char *arg, const char *store_dirname) { //11
    int res;
    char *dirname;
    int n;
    struct gnl_queue_t *queue;
    char *filename;

    print_command('w', arg);

    // parse arg
    res = arg_w_parse_arg(arg, &dirname, &n);
    GNL_MINUS1_CHECK(res, errno, -1);

    // send n files
    queue = gnl_opt_rts_scan_dir(dirname, n);
    if (queue == NULL) {
        free(dirname);
        // let the errno bubble

        return -1;
    }

    // for each dir files
    while ((filename = (char *)gnl_queue_dequeue(queue)) != NULL) {

        // send the file to the server
        res = gnl_opt_arg_send_file(filename, store_dirname);

        free(filename);

        // if an error happen stop the execution
        if (res == -1) {
            break;
        }
    }

    free(dirname);

    // destroy the queue
    gnl_queue_destroy(queue, free);

    return res;
}

/**
 * {@inheritDoc}
 */
int arg_W(const char *arg, const char *store_dirname) { //11
    int res;
    struct gnl_queue_t *queue;
    char *filename;

    print_command('W', arg);

    // initialize the queue
    queue = gnl_queue_init();
    GNL_NULL_CHECK(queue, errno, -1);

    // parse arg
    res = gnl_opt_rts_parse_file_list(arg, queue);
    GNL_MINUS1_CHECK(res, errno, -1);

    // for each given files
    while ((filename = (char *)gnl_queue_dequeue(queue)) != NULL) {

        // send the file to the server
        res = gnl_opt_arg_send_file(filename, store_dirname);

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
 * {@inheritDoc}
 */
int arg_c(const char *arg) {
    print_command('c', arg);

    return filename_arg_walk(arg, gnl_opt_arg_remove_file);
}

/**
 * {@inheritDoc}
 */
int arg_l(const char *arg) {
    print_command('l', arg);

    return filename_arg_walk(arg, gnl_opt_arg_lock_file);
}

/**
 * {@inheritDoc}
 */
int arg_u(const char *arg) {
    print_command('u', arg);

    return filename_arg_walk(arg, gnl_opt_arg_unlock_file);
}

/**
 * {@inheritDoc}
 */
int arg_r(const char *arg, const char *store_dirname) {
    int res;
    struct gnl_queue_t *queue;
    char *filename;

    print_command('r', arg);

    // initialize the queue
    queue = gnl_queue_init();
    GNL_NULL_CHECK(queue, errno, -1);

    // parse arg
    res = gnl_opt_rts_parse_file_list(arg, queue);
    GNL_MINUS1_CHECK(res, errno, -1);

    // for each given files
    while ((filename = (char *)gnl_queue_dequeue(queue)) != NULL) {

        // send the file to the server
        res = gnl_opt_arg_read_file(filename, store_dirname);

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
 * {@inheritDoc}
 */
int arg_R(const char *arg, const char *store_dirname) {
    int res;
    int n;

    GNL_TO_INT(n, arg, -1)

    // wait if we have to
    wait_milliseconds();

    // open the file on the server (with lock)
    res = gnl_fss_api_read_N_files(n, store_dirname);

    print_log("Read N files", "", res, "N=%d", n);

    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

//
//static int arg_t(const char* param) { //8
//    return 0;
//}

#undef SOCKET_ATTEMPTS_INTERVAL
#undef SOCKET_WAIT_SEC

#include <gnl_macro_end.h>