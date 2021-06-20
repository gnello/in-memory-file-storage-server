ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_colorshell.so gnl_assert.so gnl_txtenv.so gnl_print_table.so
TARGETS_PATH = ./lib

.PHONY: all clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@

clean:
	-rm -f $(TARGETS_PATH)/*.o $(TARGETS_PATH)/*.so