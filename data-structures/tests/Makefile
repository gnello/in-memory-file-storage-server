CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g
LIBS +=
TARGETS = gnl_list_test gnl_queue_test

.PHONY: all clean cleanall
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $<

clean:
	-rm -f $(TARGETS)

cleanall	: clean
	-rm -f *.o *~