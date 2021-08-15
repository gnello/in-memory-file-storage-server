# get the root dir (credits: https://stackoverflow.com/a/23324703)
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

include .env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

LIBS += -Wl,-rpath,$(DATA_STRUCTURES_LIB) -L$(DATA_STRUCTURES_LIB) -lgnl_ts_bb_queue_t -lgnl_ts_bb_stack_t
INCLUDE += -I$(DATA_STRUCTURES_INCLUDE)

TARGETS = server client
TARGETS_PATH = ./

.PHONY: all dev tests clean clean-dev server client helpers data-structures socket tests-valgrind message file-system

TARGETS_ALL = client data-structures helpers message server socket file-system

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $(TARGETS_PATH)/$@ $< $(LDFLAGS) $(LIBS)

client: data-structures server
	cd ./client && $(MAKE)

server: data-structures helpers socket file-system
	cd ./server && $(MAKE)

message:
	cd ./message && $(MAKE)

socket: message
	cd ./socket && $(MAKE)

file-system: data-structures
	cd ./file-system && $(MAKE)

helpers:
	cd ./helpers && $(MAKE)

data-structures:
	cd ./data-structures && $(MAKE)

# build all the project, test files included
dev: all helpers
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE);)

# run all the tests present in this project
tests:
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE) tests;)

# run all tests present in this project with valgrind
tests-valgrind:
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE) tests-valgrind;)

clean:
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target) && $(MAKE) clean;)

clean-dev: clean
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE) clean;)
