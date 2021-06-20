#include <stdio.h>
#include <time.h>
#include <gnl_fss_api.h>
#include <gnl_print_table.h>

#define SOCKET_TRY_EVERY_MILLISECONDS 1000
#define SOCKET_WAIT_SEC 5

static int arg_h(const char* program_name) { //7
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

    gnl_print_table("-D DIRNAME", "Store the files trashed by the Server following\n");
    gnl_print_table("", "a -w or -W option into DIRNAME.\n");

    gnl_print_table("-r FILE1[,FILE2...]", "Read FILE/s from the Server.\n");

    gnl_print_table("-R [N=0]", "Read all files stored on the Server.\n");
    gnl_print_table("", "If N is specified, read N random files from\n");
    gnl_print_table("", "the Server.\n");

    gnl_print_table("-d DIRNAME", "Store the files read from the Server following\n");
    gnl_print_table("", "a -r or -R option into DIRNAME.\n");

    gnl_print_table("-t TIME", "Wait TIME milliseconds between sequential requests\n");
    gnl_print_table("", "to the Server.\n");

    gnl_print_table("-l FILE1[,FILE2...]", "Acquire the lock on FILE/s.\n");
    gnl_print_table("-u FILE1[,FILE2...]", "Release the lock on FILE/s.\n");

    gnl_print_table("-c FILE1[,FILE2...]", "Remove FILE/s from the Server.\n");
    gnl_print_table("-p", "Print the log of the requests made to the server.\n");

    return 0;
}

/**
 * Connect to the given socket_name.
 *
 * @param socket_name   The socket where to connect to.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int arg_f(const char* socket_name) { //3
    time_t now = time(0);

    struct timespec tim;
    tim.tv_sec = now + SOCKET_WAIT_SEC;
    tim.tv_nsec = 0;

    return gnl_fss_api_open_connection(socket_name, SOCKET_TRY_EVERY_MILLISECONDS, tim);
}

//
//static int arg_w(const char* param) { //11
//    return 0;
//}
//
//static int arg_W(const char* param) { //10
//    return 0;
//}
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