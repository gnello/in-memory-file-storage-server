include .env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

LIBS += -Wl,-rpath,$(DATA_STRUCTURES_LIB) -L$(DATA_STRUCTURES_LIB) -lgnl_ts_queue_t -lgnl_ts_stack_t
INCLUDE += -I$(DATA_STRUCTURES_INCLUDE)

TARGETS = server client
TARGETS_PATH = ./

.PHONY: all dev tests clean clean-dev server client helpers data-structures socket tests-valgrind

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $(TARGETS_PATH)/$@ $< $(LDFLAGS) $(LIBS)

client: data-structures server
	cd ./client && $(MAKE)

server: helpers socket
	cd ./server && $(MAKE)

socket:
	cd ./socket && $(MAKE)

helpers:
	cd ./helpers && $(MAKE)

data-structures:
	cd ./data-structures && $(MAKE)

# build all the project, test files included
dev: all helpers
	cd ./data-structures/tests && $(MAKE)
	cd ./helpers/tests && $(MAKE)
	cd ./server/tests && $(MAKE)
	cd ./socket/tests && $(MAKE)

# run all the tests present in this project
tests:
	cd ./data-structures/tests && $(MAKE) tests
	cd ./helpers/tests && $(MAKE) tests
	cd ./server/tests && $(MAKE) tests
	cd ./socket/tests && $(MAKE) tests

# run all tests present in this project with valgrind
tests-valgrind:
	cd ./data-structures/tests && $(MAKE) tests-valgrind
	cd ./helpers/tests && $(MAKE) tests-valgrind
	cd ./server/tests && $(MAKE) tests-valgrind
	cd ./socket/tests && $(MAKE) tests-valgrind

clean:
	cd $(TARGETS_PATH) && rm -f $(TARGETS)
	cd ./helpers && $(MAKE) clean
	cd ./data-structures && $(MAKE) clean
	cd ./server && $(MAKE) clean
	cd ./client && $(MAKE) clean
	cd ./socket && $(MAKE) clean

clean-dev: clean
	cd ./data-structures/tests && $(MAKE) clean
	cd ./helpers/tests && $(MAKE) clean
	cd ./server/tests && $(MAKE) clean
	cd ./socket/tests && $(MAKE) clean
