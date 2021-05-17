#include <stdio.h>
#include <gnl_txtenv.h>
#include "./src/gnl_in_memory_filesystem.c"

int main(int argc, char * argv[]) {


    gnl_txtenv_load("./test.txt", 0)

    // instance the filesystem
    gnl_in_memory_filesystem_init(100, 100, REPOL_FIFO);


    return 0;
}