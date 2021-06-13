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

# socket library
LIBS += -Wl,-rpath,./mocks -L./mocks
INCLUDE += -I$(ROOT)$(SOCKET_INCLUDE)

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