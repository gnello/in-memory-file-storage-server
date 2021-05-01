CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_list_t.so gnl_queue_t.so gnl_ts_queue_t.so gnl_stack_t.so
TARGETSPATH = ./lib

.PHONY: all dev clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETSPATH)/lib$@ $(TARGETSPATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o $(TARGETSPATH)/$@

dev:
	cd ./tests && $(MAKE)

clean:
	cd $(TARGETSPATH) && rm -f *.o *.so