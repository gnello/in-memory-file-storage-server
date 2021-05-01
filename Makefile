CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g
LIBS += -Wl,-rpath,./dist/data-structures/lib -L./dist/data-structures/lib -lgnl_ts_queue_t
INCLUDES += -I./data-structures/includes
TARGETS = main
TARGETSPATH = ./dist
REALTARGETSPATH = $(realpath $(TARGETSPATH))

.PHONY: all dev tests clean clean-dev helpers data-structures

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(REALTARGETSPATH)/$@ $< $(LDFLAGS) $(LIBS)

helpers:
	cd ./helpers && $(MAKE)

data-structures:
	cd ./data-structures && $(MAKE)

# build all the project, test files included
dev: all
	cd ./data-structures && $(MAKE) dev

# run all the tests present in this project
tests: all helpers
	cd ./data-structures && $(MAKE) tests

clean:
	cd $(REALTARGETSPATH) && rm -f $(TARGETS)
	cd ./helpers && $(MAKE) clean
	cd ./data-structures && $(MAKE) clean

clean-dev: clean
	cd ./data-structures/tests && $(MAKE) clean
