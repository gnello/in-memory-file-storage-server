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

# data-structures library
LIBS += -Wl,-rpath,$(DATA_STRUCTURES_PATH_LIB) -L$(DATA_STRUCTURES_PATH_LIB) -lgnl_list_t
INCLUDE += -I$(DATA_STRUCTURES_PATH_INCLUDE)

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
LIBS += -Wl,-rpath,$(ROOT)$(MESSAGE_LIB) -L$(ROOT)$(MESSAGE_LIB) -lgnl_message_n -lgnl_message_s -lgnl_message_sb -lgnl_message_sn
INCLUDE += -I$(ROOT)$(MESSAGE_INCLUDE)

# file-system library
LIBS += -Wl,-rpath,$(ROOT)$(FILE_SYSTEM_LIB) -L$(ROOT)$(FILE_SYSTEM_LIB) -lgnl_simfs_file_system
INCLUDE += -I$(ROOT)$(FILE_SYSTEM_INCLUDE)

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
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning message suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind..."; valgrind --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "in use at exit\|ERROR SUMMARY\|-- no leaks"; rm "valgrind-short.txt"; echo "\n";)