CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_colorshell.so gnl_assert.so

.PHONY: all clean cleanall


all: $(TARGETS)
VPATH = src:lib

gnl_assert.so: gnl_assert.o
	$(CC) -shared -o lib/libgnl_assert.so lib/gnl_assert.o

gnl_colorshell.so: gnl_colorshell.o
	$(CC) -shared -o lib/libgnl_colorshell.so lib/gnl_colorshell.o

gnl_assert.o: gnl_assert.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC src/gnl_assert.c -o lib/gnl_assert.o

gnl_colorshell.o: gnl_colorshell.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC src/gnl_colorshell.c -o lib/gnl_colorshell.o

clean:
	-rm -f lib/*.o lib/*.so

cleanall	: clean
	-rm -f *.o *~