CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g
LIBS += -Wl,-rpath,./data-structures/lib -L./data-structures/lib -lgnl_ts_queue_t
INCLUDES += -I./data-structures/includes
TARGETS = main

.PHONY: all dev tests clean clean-dev helpers data-structures

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

helpers:
	cd ./helpers && $(MAKE)

data-structures:
	cd ./data-structures && $(MAKE)

# build all the project, test files included
dev: all
	cd ./data-structures && $(MAKE) dev

# run all the tests present in this project
tests: all
	cd ./data-structures && $(MAKE) tests

clean:
	rm -f $(TARGETS)
	cd ./helpers && $(MAKE) clean
	cd ./data-structures && $(MAKE) clean

clean-dev: clean
	cd ./data-structures/tests && $(MAKE) clean
