
#ifndef GNL_OPT_ARG_H
#define GNL_OPT_ARG_H

/**
 * Print the help message.
 *
 * @param program_name  The name of the program.
 */
extern void arg_h(const char* program_name);

/**
 * Connect to the given socket_name.
 *
 * @param socket_name   The socket where to connect to.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int arg_f_start(const char* socket_name);

/**
 * Close connection to the given socket_name.
 *
 * @param socket_name   The socket where to close the connection.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int arg_f_end(const char* socket_name);

/**
 * Recursively send n files present in the given dirname to the server.
 * If the server trashes some files, and a store_dirname is given, store
 * it into the given store_dirname.
 *
 * @param arg           The arg has the format: dirname[,n=0]. The dirname
 *                      is the root where to grab the files. If provided, n is
 *                      the number of files to send to the server, if n=0 all
 *                      the files present into dirname and in its sub-folders
 *                      will be sent to the server.
 * @param store_dirname The path where to store the trashed files from
 *                      the server.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int arg_w(const char *arg, const char *store_dirname);

/**
 * Send the given files to the server. If the server trashes some files,
 * and a store_dirname is given, store it into the given store_dirname.
 *
 * @param arg           The arg has the format: file1[,file2]. It is a
 *                      list of files that will be sent to the server.
 * @param store_dirname The path where to store the trashed files from
 *                      the server.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int arg_W(const char *arg, const char *store_dirname);

/**
 * Remove the given files from the server.
 *
 * @param arg   The arg has the format: file1[,file2]. It is a
 *              list of files that will be removed from the server.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int arg_c(const char *arg);

/**
 * Lock the given files on the server.
 *
 * @param arg   The arg has the format: file1[,file2]. It is a
 *              list of files that will be locked on the server.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int arg_l(const char *arg);

/**
 * Unlock the given files on the server.
 *
 * @param arg   The arg has the format: file1[,file2]. It is a
 *              list of files that will be locked on the server.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int arg_u(const char *arg);

/**
 * Read the given files from the server.
 *
 * @param arg           The arg has the format: file1[,file2]. It is a
 *                      list of files that will be read from the server.
 * @param store_dirname The path where to store the read files from
 *                      the server.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int arg_r(const char *arg, const char *store_dirname);

/**
 * Read the given number of files from the server. If te given number
 * was not specified, or it is equal to 0, then will be read all the
 * files present into the server.
 *
 * @param arg           The arg has the format: [N=0]. It is a
 *                      number of files wanted to read from the server.
 *                      If not specified, or if it is equal to 0, then
 *                      will be read all the files present into the server.
 * @param store_dirname The path where to store the read files from
 *                      the server.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int arg_R(const char *arg, const char *store_dirname);

#endif //GNL_OPT_ARG_H