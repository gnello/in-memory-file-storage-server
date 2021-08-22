#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gnl_fss_api.h>
#include <gnl_queue_t.h>
#include <gnl_print_table.h>
#include "./gnl_opt_rts.c"
#include "../include/gnl_opt_arg.h"
#include <gnl_macro_beg.h>

#define SOCKET_TRY_EVERY_MILLISECONDS 1000
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

    // create and open the file on the server (with lock)
    res = gnl_fss_api_open_file(filename, O_CREATE | O_LOCK);
    GNL_MINUS1_CHECK(res, errno, -1);

    // send the file to the server
    res = gnl_fss_api_write_file(filename, store_dirname);
    GNL_MINUS1_CHECK(res, errno, -1);

    // close the file
    res = gnl_fss_api_close_file(filename);
    GNL_MINUS1_CHECK(res, errno, -1);

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
    gnl_print_table("", "It must be followed by a -w or -W option.\n");

    gnl_print_table("-r FILE1[,FILE2...]", "Read FILE/s from the Server.\n");

    gnl_print_table("-R [N=0]", "Read all files stored on the Server.\n");
    gnl_print_table("", "If N is specified, read N random files from\n");
    gnl_print_table("", "the Server.\n");

    gnl_print_table("-d DIRNAME", "Store the files read from the Server into DIRNAME.\n");
    gnl_print_table("", "It must be followed by a -r or -R option.\n");

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
    time_t now = time(0);

    struct timespec tim;
    tim.tv_sec = now + SOCKET_WAIT_SEC;
    tim.tv_nsec = 0;

    return gnl_fss_api_open_connection(socket_name, SOCKET_TRY_EVERY_MILLISECONDS, tim);
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

    // parse arg
    arg_w_parse_arg(arg, &dirname, &n);

    // send n files
    queue = gnl_opt_rts_scan_dir(dirname, n);

    // for each dir files
    while ((filename = (char *)gnl_queue_dequeue(queue)) != NULL) {

        // send the file to the server
        res = gnl_opt_arg_send_file(filename, store_dirname);
        GNL_MINUS1_CHECK(res, errno, -1);

        free(filename);
    }

    free(dirname);

    // destroy the queue
    gnl_queue_destroy(queue, NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
int arg_W(const char *arg, const char *store_dirname) { //11
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

        // send the file to the server
        res = gnl_opt_arg_send_file(filename, store_dirname);

        free(filename);

        // if an error happen stop the execution
        if (res == -1) {
            break;
        }
    }

    // destroy the queue
    gnl_queue_destroy(queue, NULL);

    return res;
}

//
//static int arg_D(const char* param) { //0
//    return 0;
//}
//
//static int arg_r(const char* param) { //6
//    return 0;
//}
//
//static int arg_R(const char* param) { //5
//    return 0;
//}
//
//static int arg_d(const char* param) { //1
//    return 0;
//}
//
//static int arg_t(const char* param) { //8
//    return 0;
//}
//
//static int arg_l(const char* param) { //11
//    return 0;
//}
//
//static int arg_u(const char* param) { //9
//    return 0;
//}
//
//static int arg_c(const char* param) { //0
//    return 0;
//}
//
//static int arg_p(const char* param) { //4
//    return 0;
//}

#undef SOCKET_TRY_EVERY_MILLISECONDS
#undef SOCKET_WAIT_SEC

#include <gnl_macro_end.h>