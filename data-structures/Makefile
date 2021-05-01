CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_list_t.so gnl_queue_t.so gnl_ts_queue_t.so
TARGETSPATH = ./../dist/data-structures/lib
REALTARGETSPATH = $(realpath $(TARGETSPATH))

.PHONY: all dev copyheaders clean tests

all: $(TARGETS)
VPATH = src

%.so: %.o copyheaders
	$(CC) -shared -o $(REALTARGETSPATH)/lib$@ $(REALTARGETSPATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o $(REALTARGETSPATH)/$@

dev:
	cd ./tests && $(MAKE)

copyheaders:
	cp includes/* $(REALTARGETSPATH)/../includes

clean:
	cd $(REALTARGETSPATH) && rm -f *.o *.so && cd ../includes && rm -f *.h

tests:
	cd ./tests && $(MAKE) tests