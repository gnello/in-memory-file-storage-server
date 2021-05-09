#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <gnl_queue_t.h>

typedef int (*gnl_opt_handler)(const char *);

struct opt_handler {
    int delay;
    gnl_queue_t *command_queue;
    int debug;
};

struct opt {
    char opt;
    void *arg;
};

#define SHORT_OPTS "hf:w:W:D:r:R::d:t:l:u:c:p"

//static int arg_h(const char* param) { //7
//    return 0;
//}
//
//static int arg_f(const char* param) { //3
//    return 0;
//}
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

extern int gnl_opt_handler_init(int argc, char* argv[]) {
    struct opt_handler opt_handler;
    opt_handler.command_queue = gnl_queue_init();

    int opt;
    int temp;
    char *ptr = NULL;
    struct opt opt_el;

    while ((opt = getopt(argc, argv, SHORT_OPTS)) != -1) {
        switch (opt) {
            case 't':
                temp = strtol(optarg, &ptr, 10); //TODO: https://stackoverflow.com/questions/26080829/detecting-strtol-failure/26083517
                opt_handler.delay = temp;
                break;

            case 'p':
                opt_handler.debug = 1;
                break;

            case '?':
                printf("ah\n");
                break;

            default:
                opt_el.opt = opt;
                opt_el.arg = optarg;

                gnl_queue_enqueue(opt_handler.command_queue, (void *)&opt_el);
                break;
        }
    }

    return 0;
}

#undef SHORT_OPTS