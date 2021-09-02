
#ifndef GNL_OPT_RTS_H
#define GNL_OPT_RTS_H

/**
 * Recursively scan the given dirname and put n files in
 * the queue. If n=0 put all the found files into the queue.
 *
 * @param dirname   The root dirname to scan.
 * @param n         If n=0, put all files into the queue, otherwise
 *                  put into it only n files.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern struct gnl_queue_t *gnl_opt_rts_scan_dir(const char *dirname, int n);

/**
 * Parse the given file list sting formatted as file1[,file2] into a queue.
 *
 * @param arg       The file list sting to parse. Every file should be separate
 *                  with a comma and there should not be any white spaces.
 * @param queue     The destination queue where to store the parsed files.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_opt_rts_parse_file_list(const char *file_list, struct gnl_queue_t *queue);

#endif //GNL_OPT_RTS_H