ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic

# helpers library
LIBS = -Wl,-rpath,$(ROOT)$(HELPERS_LIB) -L$(ROOT)$(HELPERS_LIB) -lgnl_logger
INCLUDE = -I$(ROOT)$(HELPERS_INCLUDE)

# data-structures library
LIBS = -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_min_heap_t -lgnl_ternary_search_tree_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

TARGETS = gnl_simfs_file_system.so

TARGETS_PATH = ./lib

.PHONY: all clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@ $(LIBS)

clean:
	-rm -f $(TARGETS_PATH)/*.o $(TARGETS_PATH)/*.so