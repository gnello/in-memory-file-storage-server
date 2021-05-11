
#ifndef GNL_TXTENV_H
#define GNL_TXTENV_H

/**
 *
 * @param path      The path of the .txt config file.
 * @param overwrite If !=0 overwrite an existing env variable.
 *
 * @return          Returns 0 on success, -1 on failure.
 */
extern int gnl_txtenv_load(const char * path, int overwrite);

#endif //GNL_TXTENV_H