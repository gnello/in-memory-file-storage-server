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

.PHONY: all client server message socket \
		file-system helpers data-structures \
		dev tests tests-failure tests-valgrind \
		tests-valgrind-short tests-valgrind-error \
		clean clean-dev test1 test2 test3

TARGETS_ALL = client data-structures helpers message server socket file-system

VPATH = src

all: $(TARGETS)

%: %.c data-structures
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $(TARGETS_PATH)/$@ $< $(LDFLAGS) $(LIBS)

client: data-structures server
	cd ./client && $(MAKE)

server: data-structures helpers socket file-system
	cd ./server && $(MAKE)

message: data-structures
	cd ./message && $(MAKE)

socket: data-structures message
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

# run all tests present in this project and exit with an error code if test failures are found
tests-failure:
	echo "> Test failures check..."
	$(MAKE) tests >> ./tests-fail.txt
	FAILS=$$(cat tests-fail.txt | grep "FAILED" | wc -l); \
 	if [ "$$FAILS" -gt 0 ]; then echo "$${FAILS} test failures found" && rm "tests-fail.txt" && exit 1; else echo "no test failures found"; fi
	rm "tests-fail.txt"

# run all tests present in this project with valgrind
tests-valgrind:
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE) tests-valgrind;)

# run all tests present in this project with valgrind but reduce the output to the ERROR SUMMARY
tests-valgrind-short:
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE) tests-valgrind-short;)

# run all tests present in this project and exit with an error code if errors are found
tests-valgrind-error:
	echo "> Valgrind errors check..."
	$(MAKE) tests-valgrind-short >> ./valgrind-error.txt
	ERRORS=$$(cat valgrind-error.txt | grep "ERROR SUMMARY: [^0] errors" | wc -l); \
 	if [ "$$ERRORS" -gt 0 ]; then echo "$${ERRORS} errors found" && rm "valgrind-error.txt" && exit 1; else echo "no errors found"; fi
	rm "valgrind-error.txt"

clean:
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target) && $(MAKE) clean;)
	rm -f /tmp/LSOfilestorage_*.sk
	rm -f /tmp/gnl_fss*.log
	rm -f /tmp/gnl_stress_test_*

clean-dev: clean
	$(foreach target,$(TARGETS_ALL),cd $(ROOT_DIR)/$(target)/tests && $(MAKE) clean;)

test1: server client
	echo "\nRunning feature test...\n\n"
	cd ./server && valgrind --leak-check=full ./main -f ../test/config-feature-test.txt &
	cd test && ./feature_test.sh
	kill -HUP $$(ps aux | grep "./main -f ../test/config-feature-test.txt" | awk 'NR==1{print $$2}')

test2: server client
	echo "\nRunning replacement policy test...\n\n"
	cd ./server && ./main -f ../test/config-replacement-policy-test.txt &
	cd test && ./replacement_policy_test.sh
	kill -HUP $$(ps aux | grep "./main -f ../test/config-replacement-policy-test.txt" | awk 'NR==1{print $$2}')

test3: server client
	echo "\nRunning stress test...\n\n"
	cd ./server && ./main -f ../test/config-stress-test.txt &
	cd test && ./stress_test.sh
	kill -INT $$(ps aux | grep "./main -f ../test/config-stress-test.txt" | awk 'NR==1{print $$2}')

