#include <stdio.h>
#include <sc_handler.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}
//gcc -Wall -g src/main.c -o myprog -L lib/* -I lib/* -lsc_handler