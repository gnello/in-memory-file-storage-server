CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_colorshell.so gnl_assert.so

.PHONY: all clean cleanall


all: $(TARGETS)
VPATH = src

gnl_assert.so: gnl_assert.o
	$(CC) -shared -o lib/$@.so lib/gnl_assert.o

gnl_colorshell.so: gnl_colorshell.o
	$(CC) -shared -o lib/$@.so lib/gnl_colorshell.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o lib/$@

clean:
	-rm -f lib/*.o lib/*.so

cleanall	: clean
	-rm -f *.o *~