# Filesystem
The filesystem is implemented by three data structures: 

- the **File Table**: contains the inodes of all the files in the filesystem 
- the **File Descriptor Table**: contains inode copies of the open files, a file descriptor is the index of an entry in this table
- the **Inode**: contains the file information and a direct pointer to it

All the public interface methods of the filesystem are performed under a lock, this means that only one thread per time
can operate on the filesystem.

## File Table

## File Descriptor Table

## Inode

## The Filesystem Interface
As already mentioned, only one thread per time can operate on the filesystem. In addition, a thread can lock a file so 
that any other thread can not access it until the lock is released. Finally, the filesystem must ensure data integrity 
across multiple operations by different threads. To implement these requirements it is necessary to make some considerations 
on borderline cases as well as on performance. Below we discuss the precautions to be taken for all the methods of the 
public interface.

### Open a file
```c 
extern int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, const char *filename, int flags, unsigned int pid);
```
A successful opening of a file increases his reference count by one. A file can be opened in "create" and/or "lock" mode 
by providing the appropriate flag (`O_CREATE` and `O_LOCK` respectively). We discuss each case.

#### Open with no flag
In this case, the file must be present in the filesystem and it must not be locked by any other thread, that means that his 
reference count might be greater than zero. After that, it can be accessed without additional concerns. If the file is not 
present in the filesystem or is locked, then the opening will fail. In addition, there might be a thread waiting to lock
the file, or to open it with the `O_LOCK` flag, in this case the opening will fail, treating the file as if it were locked.

#### Open with `O_LOCK` flag
In this case, the file must be present in the filesystem, and it must not be locked by any other thread. In addition, it 
must not be already opened by any other thread, that means that his reference count must be equal to zero, or it may be 
greater than zero but his reference list must contain only the invoking thread. After that, it can be locked without 
additional concerns. If the file is not present or is locked by any other thread, then the opening will fail. If the file 
is opened by any other thread, the filesystem will wait until each thread closes it and his reference count shrinks to zero, 
or until his reference list will contain only the invoking thread, then it will lock the file as soon as possible.

#### Open with `O_CREATE` flag
In this case, the only concern is that the file must not be already present in the filesystem. After that, it can be
created without additional concerns. If the file is already present, then the opening will fail.

#### Open with `O_CREATE` and `O_LOCK` flags
In this case, the only concern is that the file must not be already present in the filesystem. After that, it can be 
created and locked as well without additional concerns. If the file is already present, then the opening will fail.

### Write a file
```c 
extern int gnl_simfs_file_system_write(struct gnl_simfs_file_system *file_system, int fd, const void *buf, size_t count, unsigned int pid);
```
Writing to a file is guaranteed to be atomic by the filesystem. We can write a file in two cases: the file is not locked 
or the file is locked, and we own the lock. We discuss each case.

#### Write a file that is not locked
In this case, if the file is not locked, then any thread can write the file, that means that his reference count might 
be greater than one. However, because the filesystem permits only one thread per time to operate on it, the file can be 
written without additional concerns. If the file is locked, then the writing will fail.

#### Write a file that is locked, but we own the lock
In this case, no other threads can write the file because of the lock, that means that his reference list contains only 
the invoking thread. Thus, the file can be written without additional concerns.

### Read a file
```c 
extern int gnl_simfs_file_system_read(struct gnl_simfs_file_system *file_system, int fd, void **buf, size_t *count, unsigned int pid);
```
Reading a file requires the same reasoning as writing, so we can read a file in two cases: the file is not locked
or the file is locked, and we own the lock. We discuss each case.

#### Read a file that is not locked
In this case, if the file is not locked, then any thread can read the file, that means that his reference count might 
be greater than one. Be aware that in this case any thread can also write on the file. However, because the filesystem 
permits only one thread per time to operate on it, the file can be read without additional concerns. If the file is 
locked, then the reading will fail.

#### Read a file that is locked, but we own the lock
In this case, no other threads can read the file because of the lock, that means that his reference list contains only
the invoking thread. Thus, the file can be read without additional concerns.

### Close a file
```c 
extern int gnl_simfs_file_system_close(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid);
```
A successful closing of a file decreases his reference count by one. If the file is locked by the invoking thread, then it
will be unlocked. Notice that a thread can hold more than one file descriptor per file, that means that if a thread opened
the same file twice or more, and then uses one of that file descriptors to lock the file, then the lock will be shared 
to all the others file descriptors pointing that file. Similarly, closing a file descriptor pointing to a file locked by 
the invoking thread will result in an unlocking of all the others file descriptors held by the invoking thread pointing 
that file.

### Remove a file
```c 
extern int gnl_simfs_file_system_remove(struct gnl_simfs_file_system *file_system, const char *filename, unsigned int pid);
```
We can remove a file only if it is locked, and we own the lock. Thus, we can refer to the ["Lock a file"](#lock-a-file)
or ["Open with lock flag"](#open-with-O_CREATE-flag) sections for this method precaution analysis. After that, the file 
can be removed without additional concerns. If the file is not locked, or if it is and the invoking thread does not own 
the lock, then the removing will fail.

### Lock a file
```c
extern int gnl_simfs_file_system_lock(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid);
```
To lock a file, the file must not be already locked or opened by any other thread, that means that his reference list must 
contain only the invoking thread. After that, it can be locked without additional concerns. If the file is already locked 
by the invoking thread, then nothing happen. If the file is locked by any other thread, then the locking will fail. If the 
file is opened by any other thread, then the filesystem will wait until each thread closes it and his reference list will 
contain only the invoking thread, then it will lock the file as soon as possible. 

### Unlock a file
```c
extern int gnl_simfs_file_system_unlock(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid);
```
To unlock a file, the file must be previously locked by the invoking thread, that means that his reference list must
contain only the invoking thread. After that, it can be locked without additional concerns. If the file is already 
unlocked, or if the file is locked by any other thread, so that the invoking thread does not own the lock, then the 
locking will fail.