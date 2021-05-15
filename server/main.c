#include <stdio.h>
#include "./src/gnl_in_memory_filesystem.c"

int main(int argc, char * argv[]) {
    // instance the filesystem
    gnl_in_memory_filesystem_init(100, 100, REPOL_FIFO);


    return 0;
}