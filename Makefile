include .env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

LIBS += -Wl,-rpath,$(DATA_STRUCTURES_LIB) -L$(DATA_STRUCTURES_LIB) -lgnl_ts_bb_queue_t -lgnl_ts_bb_stack_t
INCLUDE += -I$(DATA_STRUCTURES_INCLUDE)

TARGETS = server client
TARGETS_PATH = ./

.PHONY: all dev tests clean clean-dev server client helpers data-structures socket tests-valgrind message

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $(TARGETS_PATH)/$@ $< $(LDFLAGS) $(LIBS)

client: data-structures server
	cd ./client && $(MAKE)

server: data-structures helpers socket
	cd ./server && $(MAKE)

message:
	cd ./message && $(MAKE)

socket: message
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
	cd ./message/tests && $(MAKE)
	cd ./socket/tests && $(MAKE)
	cd ./client/tests && $(MAKE)

# run all the tests present in this project
tests:
	cd ./data-structures/tests && $(MAKE) tests
	cd ./helpers/tests && $(MAKE) tests
	cd ./server/tests && $(MAKE) tests
	cd ./message/tests && $(MAKE) tests
	cd ./socket/tests && $(MAKE) tests
	cd ./client/tests && $(MAKE) tests

# run all tests present in this project with valgrind
tests-valgrind:
	cd ./data-structures/tests && $(MAKE) tests-valgrind
	cd ./helpers/tests && $(MAKE) tests-valgrind
	cd ./server/tests && $(MAKE) tests-valgrind
	cd ./message/tests && $(MAKE) tests-valgrind
	cd ./socket/tests && $(MAKE) tests-valgrind
	cd ./client/tests && $(MAKE) tests-valgrind

clean:
	cd ./helpers && $(MAKE) clean
	cd ./data-structures && $(MAKE) clean
	cd ./server && $(MAKE) clean
	cd ./message && $(MAKE) clean
	cd ./socket && $(MAKE) clean
	cd ./client && $(MAKE) clean

clean-dev: clean
	cd ./data-structures/tests && $(MAKE) clean
	cd ./helpers/tests && $(MAKE) clean
	cd ./server/tests && $(MAKE) clean
	cd ./message/tests && $(MAKE) clean
	cd ./socket/tests && $(MAKE) clean
	cd ./client/tests && $(MAKE) clean
