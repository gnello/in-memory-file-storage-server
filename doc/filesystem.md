# Filesystem
The filesystem is implemented by two data structures: 

- a **File Table**: contains the inodes of all the files in the filesystem 
- a **File Descriptor Table**: contains inode copies of the open files, a file descriptor is the index of an entry in this table

All the public interface methods of the filesystem are performed under a lock, this means that only one thread per time
can operate on the filesystem.

## The File Table

## The File Descriptor Table

## Interface
As already mentioned, only one thread per time can operate on the filesystem. In addition, a thread can lock a file so 
that other threads can not access it until the lock is released. To implement these requirements it is necessary to make 
some considerations on borderline cases as well as on performance. Below we discuss the precautions to be taken for each 
method of the public interface.

### Open a file
```c 
extern int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, const char *filename, int flags, unsigned int pid);
```
A file can be opened in "create" and/or "lock" mode by providing the appropriate flag. We discuss each case.

#### Open with no flag
In this case, the file must be present in the filesystem and it must not be locked by other threads. After that, it can 
be accessed without additional concerns. If the file is not present in the filesystem or is locked, then the opening will fail.
In addition, there might be a thread waiting to open the file with the "lock" flag, in this case the opening will fail,
treating the file as if it were locked.

#### Open with "lock" flag
In this case, the file must be present in the filesystem and it must not be locked by other threads. In addition, it 
must not be already opened by other threads, including the invoking thread. After that, it can be locked without 
additional concerns. If the file is not present or is locked by other threads, then the opening will fail. If the file is 
opened by other threads, including the invoking one, the filesystem will wait until each thread closes it, then it will 
lock the file as soon as possible. Notice that if a thread open a file without lock, and then try to open the same file
providing the "lock" flag, but without closing the previous open, then the opening will fail preventing a deadlock. 

#### Open with "create" flag
In this case, the only concern is that the file must not be already present in the filesystem. After that, it can be
created without additional concerns. If the file is already present, then the opening will fail.

#### Open with "create and lock" flag
In this case, the only concern is that the file must not be already present in the filesystem. After that, it can be 
created and locked as well. If the file is already present, then the opening will fail.

### Write a file
```c 
extern int gnl_simfs_file_system_write(struct gnl_simfs_file_system *file_system, int fd, const void *buf, size_t count, unsigned int pid);
```
A write to a file is guaranteed to be atomic by the filesystem. We can write a file in two cases: the file is not locked 
or the file is locked and we own the lock. We discuss each case.

#### Write a file that is not locked
In this case, any thread can write the file. However, because the filesystem permits only one thread per time to operate 
on it, the file can be written without additional concerns. If the file is locked, then the writing will fail.

#### Write a file that is locked, but we own the lock
In this case, no other threads can write the file. Thus, the file can be written without additional concerns.
