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
be accessed without additional concerns. If the file is not present or is locked, then the open will fail.

#### Open with "lock" flag
In this case, the file must be present in the filesystem and it must not be locked by other threads. In addition, it 
must not be already opened by other threads, including the invoking thread. After that, it can be locked without 
additional concerns. If the file is not present or is locked by other threads, then the open will fail. If the file is 
opened by other threads, including the invoking one, the filesystem will wait until each thread closes it, then it will 
lock the file as soon as possible.

#### Open with "create" flag
In this case, the only concern is that the file must not be already present in the filesystem. After that, it can be
created without additional concerns. If the file is already present, then the open will fail.

#### Open with "create and lock" flag
In this case, the only concern is that the file must not be already present in the filesystem. After that, it can be 
created and locked as well. If the file is already present, then the open will fail.
