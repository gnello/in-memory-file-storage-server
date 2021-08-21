ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic

# helpers library
INCLUDE += -I$(ROOT)$(HELPERS_INCLUDE)

TARGETS =	gnl_message_n.so \
			gnl_message_s.so \
			gnl_message_sb.so \
			gnl_message_nb.so \
			gnl_message_sn.so

TARGETS_PATH = ./lib

.PHONY: all clean

all: $(TARGETS)
VPATH = src

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@ $(LIBS)

clean:
	-rm -f $(TARGETS_PATH)/*.o $(TARGETS_PATH)/*.so