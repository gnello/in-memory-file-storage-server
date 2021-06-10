ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_txtenv
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

# server library
LIBS += -Wl,-rpath,../lib -L../lib -lgnl_fss_storage
INCLUDE += -I../include

TARGETS = unit/gnl_fss_config_test unit/socket/gnl_fss_socket_request_open_test unit/socket/gnl_fss_socket_request_generic_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning server suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning server suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)