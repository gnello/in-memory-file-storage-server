ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g -D_POSIX_C_SOURCE=200809L

DATA_STRUCTURES_PATH_LIB = $(ROOT)/$(DATA_STRUCTURES_LIB)
DATA_STRUCTURES_PATH_INCLUDE = $(ROOT)/$(DATA_STRUCTURES_INCLUDE)

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

SERVER_PATH_LIB = $(ROOT)/$(SERVER_LIB)
SERVER_PATH_INCLUDE = $(ROOT)/$(SERVER_INCLUDE)

SOCKET_PATH_LIB = $(ROOT)/$(SOCKET_LIB)
SOCKET_PATH_INCLUDE = $(ROOT)/$(SOCKET_INCLUDE)

# file-system library
LIBS += -Wl,-rpath,$(ROOT)$(FILE_SYSTEM_LIB) -L$(ROOT)$(FILE_SYSTEM_LIB) -lgnl_simfs_file_system
INCLUDE += -I$(ROOT)$(FILE_SYSTEM_INCLUDE)

# helpers library
LIBS += -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_txtenv -lgnl_file_to_pointer
INCLUDE += -I$(HELPERS_PATH_INCLUDE)

# mocks library
LIBS += -Wl,-rpath,./mocks -L./mocks
LIBS += -lgnl_socket_service

# socket library
LIBS += -Wl,-rpath,$(SOCKET_PATH_LIB) -L$(SOCKET_PATH_LIB)
LIBS += -lgnl_socket_request -lgnl_socket_response
INCLUDE += -I$(SOCKET_PATH_INCLUDE)

# message library
LIBS += -Wl,-rpath,$(ROOT)$(MESSAGE_LIB) -L$(ROOT)$(MESSAGE_LIB) -lgnl_message_n -lgnl_message_s -lgnl_message_snb -lgnl_message_sn -lgnl_message_nnb -lgnl_message_nq
INCLUDE += -I$(ROOT)$(MESSAGE_INCLUDE)

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_list_t -lgnl_ternary_search_tree_t -lgnl_queue_t
LIBS += -lgnl_ts_bb_queue_t -lgnl_min_heap_t -lgnl_ternary_search_tree_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

TARGETS =	gnl_fss_config_test \
			gnl_fss_api_test

.PHONY: all clean tests tests-valgrind mocks
.SUFFIXES: .c .h

all: $(TARGETS)
VPATH = mocks

%: %.c mocks
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

mocks: gnl_socket_service.so

%.so: %.o
	$(CC) -shared -o mocks/$@ mocks/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o mocks/$@ $(LIBS)

clean:
	-rm -f $(TARGETS) && rm -f ./mocks/*.o ./mocks/*.so

tests:
	echo "\nRunning server suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning server suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind --leak-check=full --fair-sched=yes ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning server suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind (shortened output)..."; valgrind --leak-check=full --fair-sched=yes --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "ERROR SUMMARY"; rm "valgrind-short.txt"; echo "\n";)