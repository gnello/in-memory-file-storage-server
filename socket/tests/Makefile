ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

LIBS += -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_txtenv -lgnl_file_to_pointer
INCLUDE += -I$(HELPERS_PATH_INCLUDE)

# message library
LIBS += -Wl,-rpath,$(ROOT)$(MESSAGE_LIB) -L$(ROOT)$(MESSAGE_LIB) -lgnl_message_n -lgnl_message_s -lgnl_message_snb -lgnl_message_sn -lgnl_message_nnb -lgnl_message_nq
INCLUDE += -I$(ROOT)$(MESSAGE_INCLUDE)

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_queue_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

TARGETS =	gnl_socket_request_test \
			gnl_socket_response_test \
			gnl_socket_service_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS) gnl_socket_service_test_server

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning socket suite test...\n\n"
	./gnl_socket_service_test_server &
	$(foreach test,$(TARGETS),./$(test);)
	kill -9 $$(ps aux | grep "gnl_socket_service_test_server" | awk 'NR==1{print $$2}')

tests-valgrind:
	echo "\nRunning socket suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind --leak-check=full --fair-sched=yes ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning socket suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind (shortened output)..."; valgrind --leak-check=full --fair-sched=yes --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "ERROR SUMMARY"; rm "valgrind-short.txt"; echo "\n";)