
#ifndef GNL_OPT_HANLDER_H
#define GNL_OPT_HANLDER_H

/**
 * Create a new gnl_opt_handler instance.
 *
 * @return  Returns the new gnl_opt_handler created on success,
 *          NULL otherwise.
 */
extern struct gnl_opt_handler *gnl_opt_handler_init();

/**
 * Destroy the given gnl_opt_handler instance.
 *
 * @param handler   The gnl_opt_handler instance to destroy.
 */
extern void gnl_opt_handler_destroy(struct gnl_opt_handler *handler);

/**
 * Parse the command-line arguments.
 *
 * @param handler   The handler that will hold the parsed arguments.
 * @param argc      The argument count as passed to the main().
 * @param argv      The argument array as passed to the main().
 * @param opt_err   The pointer where to put the opt that caused an error,
 *                  if any. If opt_err == 0, no opt caused an error.
 * @param error     The pointer where to put the error string, if any.
 *                  If no errors occurred it is left untouched.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_opt_handler_parse_opt(struct gnl_opt_handler *handler, int argc, char* argv[], char *opt_err, char **error);

/**
 * Handle the previously parsed command-line arguments.
 *
 * @param handler   The handler holding the parsed arguments.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_opt_handler_handle(struct gnl_opt_handler *handler);

#endif //GNL_OPT_HANLDER_H