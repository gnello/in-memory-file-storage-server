ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -D_POSIX_C_SOURCE=200112L

INCLUDE = -I./include

TARGETS =	gnl_colorshell.so \
			gnl_assert.so \
			gnl_txtenv.so \
			gnl_print_table.so \
			gnl_logger.so \
			gnl_file_to_pointer.so \
			gnl_file_saver.so

TARGETS_PATH = ./lib

ifeq ($(GNL_ASSERT_BLOCK), 1)
	CFLAGS += -DGNL_ASSERT_BLOCK
endif

.PHONY: all clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@

clean:
	-rm -f $(TARGETS_PATH)/*.o $(TARGETS_PATH)/*.so