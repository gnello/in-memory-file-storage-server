include .env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

LIBS += -Wl,-rpath,$(DATA_STRUCTURES_LIB) -L$(DATA_STRUCTURES_LIB) -lgnl_ts_queue_t -lgnl_ts_stack_t
INCLUDES += -I$(DATA_STRUCTURES_INCLUDES)

TARGETS = main
#TARGETS_PATH = ./dist

.PHONY: all dev tests clean clean-dev helpers data-structures tests-valgrind

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(TARGETS_PATH)/$@ $< $(LDFLAGS) $(LIBS)

helpers:
	cd ./helpers && $(MAKE)

data-structures:
	cd ./data-structures && $(MAKE)

# build all the project, test files included
dev: all helpers
	cd ./data-structures/tests && $(MAKE)
	cd ./helpers/tests && $(MAKE)

# run all the tests present in this project
tests:
	cd ./data-structures/tests && $(MAKE) tests
	cd ./helpers/tests && $(MAKE) tests

# run all tests present in this project with valgrind
tests-valgrind:
	cd ./data-structures/tests && $(MAKE) tests-valgrind
	cd ./helpers/tests && $(MAKE) tests-valgrind

clean:
	cd $(TARGETS_PATH) && rm -f $(TARGETS)
	cd ./helpers && $(MAKE) clean
	cd ./data-structures && $(MAKE) clean

clean-dev: clean
	cd ./data-structures/tests && $(MAKE) clean
	cd ./helpers/tests && $(MAKE) clean
