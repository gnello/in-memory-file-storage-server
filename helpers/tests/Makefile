ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDES = $(ROOT)/$(HELPERS_INCLUDES)

LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert
INCLUDES = -I$(HELPERS_PATH_INCLUDES)

TARGETS = gnl_txtenv_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning helpers suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning helpers suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)