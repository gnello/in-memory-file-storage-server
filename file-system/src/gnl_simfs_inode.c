#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <gnl_huffman_tree.h>
#include "../include/gnl_simfs_inode.h"
#include <gnl_macro_beg.h>

/**
 * Compare function for the gnl_list_search method. It checks
 * if the pid is equal to the given element.
 *
 * @param el    The element of the list.
 * @param pid   The element to use to compare.
 *
 * @return      Returns 0 if the given pid is equal to the given
 *              element, -1 otherwise.
 */
static int has_pid(const void *el, const void *pid) {
    if (*(unsigned int *)el == *(unsigned int *)pid) {
        return 0;
    }

    return -1;
}

/**
 * Compare function for the gnl_list_search method. It checks
 * if the pid is not equal to the given element.
 *
 * @param el    The element of the list.
 * @param pid   The element to use to compare.
 *
 * @return      Returns 0 if the given pid is not equal to the given
 *              element, -1 otherwise.
 */
static int has_different_pid(const void *el, const void *pid) {
    if (*(unsigned int *)el != *(unsigned int *)pid) {
        return 0;
    }

    return -1;
}

/**
 * Compress the given inode. This invocation will rewrite every
 * bytes within the given inode, and will change the size of
 * the inode.
 *
 * @param inode The inode to compress.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
static int compress(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // compress
    struct gnl_huffman_tree_artifact *artifact = gnl_huffman_tree_encode(inode->direct_ptr, inode->size);
    GNL_NULL_CHECK(artifact, errno, -1)

    // rewrite the inode
    free(inode->direct_ptr);

    inode->size = gnl_huffman_tree_size(artifact);
    GNL_MINUS1_CHECK(inode->size, errno, -1)

    inode->direct_ptr = artifact;

    return 0;
}

/**
 * Decompress the given inode.
 *
 * @param inode The inode to decompress.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
static int decompress(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // get the artifact
    struct gnl_huffman_tree_artifact *artifact = inode->direct_ptr;

    void *bytes;
    size_t count;

    // decompress
    int res = gnl_huffman_tree_decode(artifact, &bytes, &count);
    GNL_MINUS1_CHECK(res, errno, -1)

    // rewrite the inode
    inode->size = count;
    inode->direct_ptr = bytes;

    return 0;
}

/**
 * Destroy the given inode. This method it supposed to be called with with_pointed_file=1
 * when the intention is to destroy the inode, with with_pointed_file=0 if the intention
 * is to destroy its copy. This difference it is necessary because the copy of the inode
 * contains a direct pointer to the original file within the inode from which the copy was
 * made, so it prevents accidental deletions.
 *
 * @param inode             The inode instance to destroy.
 * @param with_pointed_file If > 0, an invocation to this method will delete
 *                          also the file pointed by the given inode, If <= 0
 *                          the pointed file will be preserved.
 */
static void destroy_inode(struct gnl_simfs_inode *inode, int with_pointed_file) {
    if (inode == NULL) {
        return;
    }

    // destroy the name
    free(inode->name);

    // destroy the file pointer
    if (with_pointed_file > 0) {
        gnl_huffman_tree_destroy_artifact(inode->direct_ptr);
        inode->direct_ptr = NULL;
    }

    // destroy the reference list
    gnl_list_destroy(&(inode->reference_list), free);
    inode->reference_list = NULL;

    // destroy the buffer
    free(inode->buffer);
    inode->buffer = NULL;

    // clear the waiting queue
//    pthread_cond_broadcast(&(inode->file_is_lockable));

    // destroy the condition variables
    pthread_cond_destroy(&(inode->file_is_lockable));

    // useless, but consistent until the end :)
    inode->ctime = time(NULL);

    // destroy the inode
    free(inode);
}

/**
 * {@inheritDoc}
 */
struct gnl_simfs_inode *gnl_simfs_inode_init(const char *name) {
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(inode, ENOMEM, NULL)

    // set the creation time of the file
    inode->btime = time(NULL);

    // initialize condition variables
    int res = pthread_cond_init(&(inode->file_is_lockable), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // set the name
    GNL_CALLOC(inode->name, strlen(name) + 1, NULL)
    strncpy(inode->name, name, strlen(name));

    // initialize others attributes
    inode->mtime = 0;
    inode->atime = 0;
    inode->size = 0;
    inode->locked = 0;
    inode->direct_ptr = NULL;
    inode->pending_locks = 0;
    inode->reference_count = 0;
    inode->reference_list = NULL;
    inode->buffer = NULL;
    inode->buffer_size = 0;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return inode;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode) {
    destroy_inode(inode, 1);
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_inode_copy_destroy(struct gnl_simfs_inode *inode) {
    destroy_inode(inode, 0);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_is_file_locked(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    return inode->locked;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_wait_file_lockability(struct gnl_simfs_inode *inode, pthread_mutex_t *mtx) {
    return pthread_cond_wait(&(inode->file_is_lockable), mtx);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_increase_refs(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // copy the pid
    unsigned int *pid_copy = malloc(sizeof(unsigned int));
    GNL_NULL_CHECK(pid_copy, ENOMEM, -1)

    *pid_copy = pid;

    // add the pid to the reference list
    int res = gnl_list_insert(&(inode->reference_list), pid_copy);
    GNL_MINUS1_CHECK(res, errno, -1);

    // increase the reference count
    inode->reference_count++;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_decrease_refs(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    if (inode->reference_count == 0) {
        errno = EPERM;
        return -1;
    }

    // check if the pid is allowed to decrease the refs
    if (gnl_list_search(inode->reference_list, &pid, has_pid) == 0) {
        errno = EPERM;
        return -1;
    }

    // remove the pid from the reference list
    int res = gnl_list_delete(&(inode->reference_list), &pid, has_pid, free);
    GNL_MINUS1_CHECK(res, errno, -1);

    // decrease the reference count
    inode->reference_count--;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    // wake up eventually waiting threads
    if (inode->reference_count == 0 || gnl_list_search(inode->reference_list, &pid, has_different_pid) == 1) {
        return pthread_cond_signal(&(inode->file_is_lockable));
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_has_refs(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    return inode->reference_count > 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_has_other_pid_refs(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // check if the pid is allowed to check the refs
    if (gnl_list_search(inode->reference_list, &pid, has_pid) == 0) {
        errno = EPERM;
        return -1;
    }

    return gnl_list_search(inode->reference_list, &pid, has_different_pid);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_file_lock(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // if the file is already locked by the
    // given pid return with success (idempotency)
    if (inode->locked == pid) {
        return 0;
    }

    // if the file is locked by a different
    // pid return an error
    if (inode->locked != 0) {
        errno = EACCES;
        return -1;
    }

    // lock the file
    inode->locked = pid;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_file_unlock(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // if the file is already unlocked
    // return with success (idempotency)
    if (inode->locked == 0) {
        return 0;
    }

    // if the file is locked by a different
    // pid return an error
    if (inode->locked != pid) {
        errno = EACCES;
        return -1;
    }

    // unlock the file
    inode->locked = 0;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_increase_pending_locks(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    inode->pending_locks++;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_decrease_pending_locks(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    if (inode->pending_locks == 0) {
        errno = EPERM;
        return -1;
    }

    inode->pending_locks--;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_has_pending_locks(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    return inode->pending_locks > 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_write(struct gnl_simfs_inode *inode, const void *buf, size_t count) {
    //validate the parameters
    GNL_NULL_CHECK(inode, EINVAL, -1)
    GNL_NULL_CHECK(buf, EINVAL, -1)

    // if we do not have to write data, return with an error
    GNL_MINUS1_CHECK(-1 * (count <= 0), EINVAL, -1)

    // calculate the new size
    int new_size = inode->buffer_size + count;

    // realloc the memory onto the buffer for the writing
    void *temp = realloc(inode->buffer, new_size);

    // do not handle errors but bubble it, if an
    // update fails we are ok with the fact that
    // realloc does not free the original pointer
    GNL_NULL_CHECK(temp, errno, -1)

    // update the original pointer if it has changed (or not)
    inode->buffer = temp;

    // write the data
    memcpy((char *)inode->buffer + inode->buffer_size, buf, count);

    // update the size of the buffer
    inode->buffer_size = new_size;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return count;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_read(struct gnl_simfs_inode *inode, void **buf, size_t *count) {
    //validate the parameters
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // decompress the inode
    int res = decompress(inode);
    GNL_MINUS1_CHECK(res, errno, -1);

    // alloc the memory onto the buffer for the reading
    *buf = calloc(inode->size, 1);
    GNL_NULL_CHECK(*buf, ENOMEM, -1)

    // read the data
    memcpy(*buf, inode->direct_ptr, inode->size);

    // set the count
    *count = inode->size;

    // set the access timestamp of the inode
    inode->atime = time(NULL);

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    // compress the inode
    res = compress(inode);
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_simfs_inode *gnl_simfs_inode_copy(const struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, NULL)

    // allocate space for the new inode
    struct gnl_simfs_inode *inode_copy = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(inode_copy, ENOMEM, NULL)

    // create a deep copy of the given inode
    inode_copy->btime = inode->btime;
    inode_copy->mtime = inode->mtime;
    inode_copy->atime = inode->atime;
    inode_copy->size = inode->size;

    GNL_CALLOC(inode_copy->name, strlen(inode->name) + 1, NULL)
    strcpy(inode_copy->name, inode->name);

    inode_copy->direct_ptr = inode->direct_ptr;
    inode_copy->locked = inode->locked;
    inode_copy->reference_count = inode->reference_count;
    inode_copy->reference_list = NULL;
    inode_copy->pending_locks = inode->pending_locks;

    // do not preserve the buffer
    inode_copy->buffer = NULL;
    inode_copy->buffer_size = 0;

    // initialize condition variables
    int res = pthread_cond_init(&(inode_copy->file_is_lockable), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // set the last status change timestamp of the inode
    inode_copy->ctime = time(NULL);

    return inode_copy;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_fflush(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    int res;

    // decompress the inode
    if (inode->direct_ptr != NULL) {
        res = decompress(inode);
        GNL_MINUS1_CHECK(res, errno, -1);
    }

    // calculate the new size
    int new_size = inode->size + inode->buffer_size;

    // re-alloc the memory onto the direct pointer for the writing
    void *temp = realloc(inode->direct_ptr, new_size);

    // do not handle errors but bubble it, if an
    // update fails we are ok with the fact that
    // realloc does not free the original pointer
    GNL_NULL_CHECK(temp, ENOMEM, -1)

    // update the original pointer if it has changed (or not)
    inode->direct_ptr = temp;

    // write the data
    memcpy((char *)inode->direct_ptr + inode->size, inode->buffer, inode->buffer_size);

    // update the size of the file within the inode
    inode->size = new_size;

    // free the buffer
    free(inode->buffer);
    inode->buffer = NULL;

    // reset the buffer size
    inode->buffer_size = 0;

    // update the last modification timestamp of the file
    // within the inode
    inode->mtime = time(NULL);

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    // compress the inode
    res = compress(inode);
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

#include <gnl_macro_end.h>
