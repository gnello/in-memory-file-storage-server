
#ifndef GNL_OPT_HANLDER_H
#define GNL_OPT_HANLDER_H

typedef struct gnl_opt_handler gnl_opt_handler;

extern struct gnl_opt_handler *gnl_opt_handler_init(int argc, char* argv[]);

extern void gnl_opt_handler_destroy(gnl_opt_handler *handler);

#endif //GNL_OPT_HANLDER_H