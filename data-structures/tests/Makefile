ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

# helpers library
LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

TARGETS =	gnl_list_test \
			gnl_queue_test \
			gnl_ts_bb_queue_test \
			gnl_ts_nb_queue_test \
			gnl_stack_test \
			gnl_ts_bb_stack_test \
			gnl_min_heap_test \
			gnl_ternary_search_tree_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning data structures suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning data structures suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind --leak-check=full --fair-sched=yes ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning data structures suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind (shortened output)..."; valgrind --leak-check=full --fair-sched=yes --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "ERROR SUMMARY"; rm "valgrind-short.txt"; echo "\n";)