ROOT=../../

include $(ROOT)/.env
export

CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g

HELPERS_PATH_LIB = $(ROOT)/$(HELPERS_LIB)
HELPERS_PATH_INCLUDE = $(ROOT)/$(HELPERS_INCLUDE)

LIBS = -Wl,-rpath,$(HELPERS_PATH_LIB) -L$(HELPERS_PATH_LIB) -lgnl_colorshell -lgnl_assert -lgnl_file_to_pointer
INCLUDE = -I$(HELPERS_PATH_INCLUDE)

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_queue_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

TARGETS =	gnl_message_s_test \
			gnl_message_n_test \
			gnl_message_sn_test \
			gnl_message_nq_test \
			gnl_message_snb_test \
			gnl_message_nnb_test

.PHONY: all clean tests tests-valgrind
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	-rm -f $(TARGETS)

tests:
	echo "\nRunning message suite test...\n\n"
	$(foreach test,$(TARGETS),./$(test);)

tests-valgrind:
	echo "\nRunning message suite test...\n\n"
	$(foreach test,$(TARGETS),echo "\n> running $(test) with valgrind...\n"; valgrind --leak-check=full --fair-sched=yes ./$(test); echo "\n";)

tests-valgrind-short:
	echo "\nRunning message suite test...\n"
	$(foreach test,$(TARGETS),echo "> running $(test) with valgrind (shortened output)..."; valgrind --leak-check=full --fair-sched=yes --log-file="valgrind-short.txt" ./$(test) > /dev/null; cat valgrind-short.txt | grep "ERROR SUMMARY"; rm "valgrind-short.txt"; echo "\n";)