ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g -D_POSIX_C_SOURCE=200112L

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

DATA_STRUCTURES_PATH_LIB = $(ROOT)/$(DATA_STRUCTURES_LIB)
DATA_STRUCTURES_PATH_INCLUDE = $(ROOT)/$(DATA_STRUCTURES_INCLUDE)

# helpers library
LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

# data-structures library
LIBS += -Wl,-rpath,$(DATA_STRUCTURES_PATH_LIB) -L$(DATA_STRUCTURES_PATH_LIB) -lgnl_queue_t
INCLUDE += -I$(DATA_STRUCTURES_PATH_INCLUDE)

TARGETS = gnl_opt_rts_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning helpers suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning helpers suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning message suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind..."; valgrind --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "in use at exit\|ERROR SUMMARY\|-- no leaks"; rm "valgrind-short.txt"; echo "\n";)