ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

TARGETS = 	gnl_list_t.so \
			gnl_queue_t.so \
			gnl_ts_bb_queue_t.so \
			gnl_stack_t.so \
			gnl_ts_bb_stack_t.so \
			gnl_min_heap_t.so \
			gnl_ternary_search_tree_t.so

TARGETS_PATH = ./lib

.PHONY: all dev clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@

clean:
	cd $(TARGETS_PATH) && rm -f *.o *.so