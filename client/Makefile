ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g -D_POSIX_C_SOURCE=200112L

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_queue_t -lgnl_ternary_search_tree_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

# helpers library
LIBS += -Wl,-rpath,$(ROOT)$(HELPERS_LIB) -L$(ROOT)$(HELPERS_LIB) -lgnl_print_table
INCLUDE += -I$(ROOT)$(HELPERS_INCLUDE)

# server library
LIBS += -Wl,-rpath,$(ROOT)$(SERVER_LIB) -L$(ROOT)$(SERVER_LIB) -lgnl_fss_api
INCLUDE += -I$(ROOT)$(SERVER_INCLUDE)

TARGETS = main

.PHONY: all clean

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGETS)