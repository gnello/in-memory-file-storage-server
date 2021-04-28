CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g
LIBS += -Wl,-rpath,./../../helpers/lib -L./../../helpers/lib -lgnl_colorshell -lgnl_assert
INCLUDES += -I./../../helpers/includes
TARGETS = gnl_list_test gnl_queue_test gnl_ts_queue_test

.PHONY: all clean cleanall
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)