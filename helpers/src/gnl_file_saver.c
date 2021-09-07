#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/gnl_macro_beg.h"
#include "../include/gnl_file_saver.h"

/**
 * Create the pathname specified into dir.
 *
 * This method was seen at the link:
 * http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
 *
 * @param dir   The pathname to create.
 */
static void nion_mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);

    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
}

/**
 * {@inheritDoc}
 */
int gnl_file_saver_save(const char *filename, const char *dirname, void *buf, size_t count) {
    // validate parameters
    GNL_NULL_CHECK(filename, EINVAL, -1);
    GNL_NULL_CHECK(dirname, EINVAL, -1);

    // check if dirname exists
    DIR *dir = opendir(dirname);
    GNL_NULL_CHECK(dir, errno, -1)

    closedir(dir);

    // create the complete path
    char *complete_path;
    unsigned long len = strlen(dirname) + strlen(filename) + 1;
    GNL_CALLOC(complete_path, len, -1)

    snprintf(complete_path, len, "%s%s", dirname, filename);

    // create filename directories if necessary
    nion_mkdir(complete_path);

    // write the file
    FILE *fp;

    fp = fopen(complete_path , "w");
    GNL_NULL_CHECK(fp, errno, -1)

    unsigned long nwrite = fwrite(buf, 1, count, fp);
    GNL_MINUS1_CHECK(nwrite, errno, -1)

    fclose(fp);

    // free memory
    free(complete_path);

    return 0;
}

#include "../include/gnl_macro_end.h"