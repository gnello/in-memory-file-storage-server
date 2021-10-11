
#ifndef GNL_FSS_SERVER_API_H
#define GNL_FSS_SERVER_API_H

#include <time.h>

const int O_CREATE = 1;
const int O_LOCK = 2;

/**
 * Open an AF_UNIX connection to sockname.
 * On fail attempt to connect every msec milliseconds until abstime.
 *
 * @param sockname  The socket filename.
 * @param msec      The amount of milliseconds after to re-attempt to connect.
 * @param abstime   The absolute time, if reached it stops the re-attempts.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int openConnection(const char *sockname, int msec, const struct timespec abstime);

/**
 * Close the AF_UNIX connection opened on sockname.
 *
 * @param sockname  The socket filename.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int closeConnection(const char *sockname);

/**
 * Open a file on the server.
 * Return an error if the flag O_CREATE is given and the file exists, or if O_CREATE
 * is not given and the file does not exists.
 *
 * @param pathname  The location of the file on the server.
 * @param flags     If O_CREATE is given, the file will be opened with read and write
 *                  permissions. If O_LOCK is given, the file will be opened with a lock
 *                  and it can be read or written only from the process who locked it.
 *                  O_CREATE and O_LOCK can be used together (bitwise OR).
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int openFile(const char *pathname, int flags);

/**
 * Read a file from the server.
 *
 * @param pathname  The location of the file on the server.
 * @param buf       The pointer to the file read from the server.
 * @param size      The size in bytes of the file read from the server.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int readFile(const char *pathname, void **buf, size_t *size);

/**
 * Read any N files from the server, if the server has less than N files,
 * they will all be read.
 *
 * @param N         The number of files to read, if N<=0 all the existing files will be read.
 * @param dirname   The directory where to store the files read.
 *
 * @return          Returns the number of file read on success, -1 otherwise.
 */
extern int readNFiles(int N, const char *dirname);

/**
 * Write a file to the server.
 * Return success only if the previous operation on the file was
 * openFile(pathname, O_CREATE| O_LOCK).
 *
 * @param pathname  The path of the file to write on the server.
 * @param dirname   The path where to store the eventual trashed file from the server.
 *                  If NULL is given, the eventual trashed file will be stored nowhere.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int writeFile(const char *pathname, const char *dirname);

/**
 * Append a file to the server, this operation is atomic.
 *
 * @param pathname  The location of the file on the server.
 * @param buf       The data to append to the file.
 * @param size      The size of the data to append to the file.
 * @param dirname   The path where to store the eventual trashed file from the server.
 *                  If NULL is given, the eventual trashed file will be stored nowhere.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int appendToFile(const char *pathname, void *buf, size_t size, const char *dirname);

/**
 * Acquire the lock on a file.
 * Return immediately success if the file was opened with the O_LOCK flag and the
 * request is made from the same process or if the file does not have the O_LOCK flag.
 * If the file is locked by another process, wait until the lock will be released.
 *
 * @param pathname  The location of the file on the server.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int lockFile(const char *pathname);

/**
 * Release the lock from a file.
 * Return success only if the lock owner is the same process who make the request.
 *
 * @param pathname  The location of the file on the server.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int unlockFile(const char *pathname);

/**
 * Close a file opened on the server, all the following operations
 * on the given file will fail.
 *
 * @param pathname  The location of the file on the server.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int closeFile(const char *pathname);

/**
 * Remove a file from the server.
 * Return error if the file is not locked or if the file is locked by another process.
 *
 * @param pathname  The location of the file on the server.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int removeFile(const char *pathname);

#endif //GNL_FSS_SERVER_API_H