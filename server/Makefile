ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic

# data-structures library
LIBS = -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_list_t -lgnl_queue_t
INCLUDE = -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

# helpers library
LIBS += -Wl,-rpath,$(ROOT)$(HELPERS_LIB) -L$(ROOT)$(HELPERS_LIB) -lgnl_txtenv -lgnl_print_table -lgnl_logger
INCLUDE += -I$(ROOT)$(HELPERS_INCLUDE)

# socket library
LIBS += -Wl,-rpath,$(ROOT)$(SOCKET_LIB) -L$(ROOT)$(SOCKET_LIB) -lgnl_socket_request -lgnl_socket_response -lgnl_socket_service
INCLUDE += -I$(ROOT)$(SOCKET_INCLUDE)

INCLUDE += -I./include

TARGETS = main gnl_fss_api.so

TARGETS_PATH = ./lib

.PHONY: all clean

all: $(TARGETS)
VPATH = src

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

%.so: %.o
	$(CC) -shared $(INCLUDE) -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$< $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@ $(LIBS)

clean:
	rm -f $(TARGETS)
	cd $(TARGETS_PATH) && rm -f *.o *.so