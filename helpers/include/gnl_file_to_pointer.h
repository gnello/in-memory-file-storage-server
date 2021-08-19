
#ifndef GNL_FILE_TO_POINTER_H
#define GNL_FILE_TO_POINTER_H

/**
 * Put the content of the file located at the given filepath on the heap
 * and return a pointer to it in the given pointer.
 *
 * @param filepath  The path of the file to read.
 * @param pointer   The pointer where to put the file read.
 * @param size      The pointer where to store the size of the file read.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_file_to_pointer(char *filepath, char **pointer, long *size);

#endif //GNL_FILE_TO_POINTER_H
