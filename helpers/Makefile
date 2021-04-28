CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_colorshell.so gnl_assert.so

.PHONY: all clean cleanall

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o lib/lib$@ lib/$<.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o lib/$@.o

clean:
	-rm -f lib/*.o lib/*.so