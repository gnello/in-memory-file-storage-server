ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic

# helpers library
INCLUDE = -I$(ROOT)$(HELPERS_INCLUDE)

# data-structures library
LIBS = -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_queue_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

# message library
LIBS = -Wl,-rpath,$(ROOT)$(MESSAGE_LIB) -L$(ROOT)$(MESSAGE_LIB) -lgnl_message_n -lgnl_message_s -lgnl_message_snb -lgnl_message_sn -lgnl_message_nnb
INCLUDE += -I$(ROOT)$(MESSAGE_INCLUDE)

TARGETS =	gnl_socket_request.so \
			gnl_socket_response.so \
			gnl_socket_service.so

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