ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_txtenv
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

TARGETS =	gnl_socket_message_s_test \
			gnl_socket_message_n_test \
			gnl_socket_message_sn_test \
			gnl_socket_message_sb_test \
			gnl_socket_request_test \
			gnl_socket_service_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS) gnl_socket_service_test_server

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning server suite test...\n\n"
	./gnl_socket_service_test_server &
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning server suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)