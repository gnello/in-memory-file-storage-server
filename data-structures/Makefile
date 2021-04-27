CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_list_t.so gnl_queue_t.so

.PHONY: all clean cleanall

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o lib/lib$@ lib/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o lib/$@

clean:
	-rm -f lib/*.o lib/*.so

cleanall	: clean
	-rm -f *.o *~