ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic

# data-structures library
LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_list_t -lgnl_queue_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

# helpers library
LIBS += -Wl,-rpath,$(ROOT)$(HELPERS_LIB) -L$(ROOT)$(HELPERS_LIB) -lgnl_txtenv
INCLUDE += -I$(ROOT)$(HELPERS_INCLUDE)

# server library
LIBS += -Wl,-rpath,./lib -L./lib -lgnl_fss_opt_handler -lgnl_fss_api -lgnl_fss_config -lgnl_fss_storage
INCLUDE += -I./include

TARGETS = gnl_fss_socket_open.a gnl_fss_socket_generic.a gnl_fss_api.so gnl_fss_config.so gnl_fss_opt_handler.so gnl_fss_storage.so main
TARGETS_PATH = ./lib

.PHONY: all dev clean

all: $(TARGETS)
VPATH = src:src/socket

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

%.a: %.o
	ar rvs $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@ $(LIBS)

clean:
	cd $(TARGETS_PATH) && rm -f *.o *.so