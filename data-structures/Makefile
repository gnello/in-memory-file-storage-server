CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_list_t.so gnl_queue_t.so gnl_ts_queue_t.so
TARGETSPATH = ../dist/data-structures/lib
REALTARGETSPATH = $(realpath $(TARGETSPATH))

.PHONY: all dev clean tests

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(REALTARGETSPATH)/lib$@ $(REALTARGETSPATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o $(REALTARGETSPATH)/$@

dev:
	cd ./tests && $(MAKE)

clean:
	cd $(REALTARGETSPATH) && rm -f *.o *.so

tests:
	cd ./tests && $(MAKE) tests