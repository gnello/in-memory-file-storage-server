ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_imfs_server_api.so
TARGETS_PATH = ./lib

.PHONY: all dev clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@

clean:
	cd $(TARGETS_PATH) && rm -f *.o *.so