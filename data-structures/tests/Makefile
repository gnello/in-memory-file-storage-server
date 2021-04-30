CC = gcc
CFLAGS += -std=c99 -Wall -pedantic -g
LIBS += -Wl,-rpath,./../../helpers/lib -L./../../helpers/lib -lgnl_colorshell -lgnl_assert
INCLUDES += -I./../../helpers/includes
TARGETS = gnl_list_test gnl_queue_test gnl_ts_queue_test
TARGETSPATH = ../../dist/data-structures/tests
REALTARGETSPATH = $(realpath $(TARGETSPATH))

.PHONY: all clean helpers tests
.SUFFIXES: .c .h

all: $(TARGETS)

%: %.c helpers
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(REALTARGETSPATH)/$@ $< $(LDFLAGS) $(LIBS)

helpers:
	cd ./../../helpers && $(MAKE)

clean:
	cd $(REALTARGETSPATH) && rm -f $(TARGETS)

tests:
	@echo "\nRunning data structures suite test...\n\n"
	@$(foreach test,$(TARGETS),./$(REALTARGETSPATH)/$(test);)