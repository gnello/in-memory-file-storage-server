CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_list_t.so gnl_queue_t.so gnl_ts_queue_t.so
TARGETPATH = ../dist/data-structures/lib

.PHONY: all dev clean tests

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETPATH)/lib$@ $(TARGETPATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o $(TARGETPATH)/$@

dev:
	cd ./tests && $(MAKE)

clean:
	-rm -f lib/*.o lib/*.so

tests:
	cd ./tests && $(MAKE) tests