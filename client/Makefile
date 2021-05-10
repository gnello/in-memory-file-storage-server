ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

LIBS += -Wl,-rpath,$(ROOT)/$(DATA_STRUCTURES_LIB) -L$(ROOT)/$(DATA_STRUCTURES_LIB) -lgnl_queue_t
INCLUDES += -I$(ROOT)/$(DATA_STRUCTURES_INCLUDES)

TARGETS = main

.PHONY: all clean

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGETS)