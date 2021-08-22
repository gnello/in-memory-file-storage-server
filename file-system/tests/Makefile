ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

# helpers library
LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_txtenv -lgnl_file_to_pointer -lgnl_logger
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_min_heap_t -lgnl_ternary_search_tree_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

TARGETS = gnl_simfs_inode_test gnl_simfs_file_descriptor_table_test gnl_simfs_file_system_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning file system suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning file system suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning file system suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind..."; valgrind --leak-check=full --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "ERROR SUMMARY"; rm "valgrind-short.txt"; echo "\n";)
