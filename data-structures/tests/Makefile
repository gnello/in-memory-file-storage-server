CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g
TARGETS = gnl_list_test gnl_queue_test gnl_ts_queue_test gnl_stack_test gnl_ts_stack_test
HELPERSPATH = ./../../helpers
LIBS += -Wl,-rpath,$(HELPERSPATH)/lib -L$(HELPERSPATH)/lib -lgnl_colorshell -lgnl_assert
INCLUDES += -I$(HELPERSPATH)/includes

.PHONY: all clean helpers tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c helpers
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

helpers:
	cd ./../../helpers && $(MAKE)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning data structures suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning data structures suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind ./$(test); echo "\n";)