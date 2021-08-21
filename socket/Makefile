ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic

# helpers library
INCLUDE = -I$(ROOT)$(HELPERS_INCLUDE)

# message library
LIBS = -Wl,-rpath,$(ROOT)$(MESSAGE_LIB) -L$(ROOT)$(MESSAGE_LIB) -lgnl_message_n -lgnl_message_s -lgnl_message_sb -lgnl_message_sn -lgnl_message_nb
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