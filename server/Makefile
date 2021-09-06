ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic -D_POSIX_C_SOURCE=200809L

# file-system library
LIBS += -Wl,-rpath,$(ROOT)$(FILE_SYSTEM_LIB) -L$(ROOT)$(FILE_SYSTEM_LIB) -lgnl_simfs_file_system
INCLUDE += -I$(ROOT)$(FILE_SYSTEM_INCLUDE)

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_list_t -lgnl_ternary_search_tree_t
LIBS += -lgnl_ts_bb_queue_t -lgnl_min_heap_t -lgnl_ternary_search_tree_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

# helpers library
LIBS += -Wl,-rpath,$(ROOT)$(HELPERS_LIB) -L$(ROOT)$(HELPERS_LIB) -lgnl_txtenv -lgnl_print_table -lgnl_logger -lgnl_file_to_pointer
INCLUDE += -I$(ROOT)$(HELPERS_INCLUDE)

# socket library
LIBS += -Wl,-rpath,$(ROOT)$(SOCKET_LIB) -L$(ROOT)$(SOCKET_LIB) -lgnl_socket_request -lgnl_socket_response -lgnl_socket_service
INCLUDE += -I$(ROOT)$(SOCKET_INCLUDE)

# message library
LIBS += -Wl,-rpath,$(ROOT)$(MESSAGE_LIB) -L$(ROOT)$(MESSAGE_LIB) -lgnl_message_n -lgnl_message_s -lgnl_message_snb -lgnl_message_sn -lgnl_message_nnb -lgnl_message_nq
INCLUDE += -I$(ROOT)$(MESSAGE_INCLUDE)

# add thread support
LIBS += -lpthread

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