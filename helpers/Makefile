CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_colorshell.so gnl_assert.so
TARGETSPATH = ./lib

.PHONY: all clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETSPATH)/lib$@ $(TARGETSPATH)/$<.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o $(TARGETSPATH)/$@.o

clean:
	-rm -f $(TARGETSPATH)/*.o $(TARGETSPATH)/*.so