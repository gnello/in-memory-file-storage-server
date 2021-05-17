
#ifndef GNL_FSS_OPT_HANDLER_H
#define GNL_FSS_OPT_HANDLER_H

/**
 *
 * @param argc      The main argc.
 * @param argv      The main argv.
 * @param filepath  The pointer where to put the socket filepath read from the stdin.
 * @param opt_err   The pointer where to put the option that caused an error.
 * @param error     The pointer where to put the string error.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_fss_opt_handler_parse(int argc, char* argv[], char **filepath, char *opt_err, char **error);

#endif //GNL_FSS_OPT_HANDLER_H