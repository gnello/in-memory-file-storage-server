ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

SERVER_PATH_LIB = $(ROOT)/$(SERVER_LIB)
SERVER_PATH_INCLUDE = $(ROOT)/$(SERVER_INCLUDE)

SOCKET_PATH_LIB = $(ROOT)/$(SOCKET_LIB)
SOCKET_PATH_INCLUDE = $(ROOT)/$(SOCKET_INCLUDE)

LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_txtenv
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

# server library
LIBS += -Wl,-rpath,$(SERVER_PATH_LIB) -L$(SERVER_PATH_LIB) -lgnl_fss_storage
INCLUDE += -I$(SERVER_PATH_INCLUDE)

# socket library
LIBS += -Wl,-rpath,./mocks -L./mocks -Wl,-rpath,$(SOCKET_PATH_LIB) -L$(SOCKET_PATH_LIB) -lgnl_fss_socket_request
INCLUDE += -I$(SOCKET_PATH_INCLUDE)

TARGETS =	gnl_fss_config_test \
			gnl_fss_api_test

.PHONY: all clean tests tests-valgrind mocks
.SUFFIXES: .c .h

all: $(TARGETS)
VPATH = mocks

%: %.c mocks
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

mocks: gnl_fss_socket_service.so

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