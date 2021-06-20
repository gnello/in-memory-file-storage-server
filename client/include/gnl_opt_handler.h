
#ifndef GNL_OPT_HANLDER_H
#define GNL_OPT_HANLDER_H

extern struct gnl_opt_handler *gnl_opt_handler_init();

extern void gnl_opt_handler_destroy(struct gnl_opt_handler *handler);

extern int gnl_opt_handler_parse_opt(struct gnl_opt_handler *handler, int argc, char* argv[], char *opt_err, char **error);

extern int gnl_opt_handler_handle(struct gnl_opt_handler *handler);

#endif //GNL_OPT_HANLDER_H