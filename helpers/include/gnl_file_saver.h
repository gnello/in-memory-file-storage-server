
#ifndef GNL_FILE_SAVER_H
#define GNL_FILE_SAVER_H

/**
 * Save count bytes of the given buf into filename within dirname.
 * If the path specified into the given dirname does not exist, the
 * function will return an error.
 *
 * @param filename  The name of the file to create.
 * @param dirname   The path where to create the file
 * @param buf       The pointer to the bytes of the file.
 * @param count     The number of bytes of the file to save.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_file_saver_save(const char *filename, const char *dirname, void *buf, size_t count);

#endif //GNL_FILE_SAVER_H