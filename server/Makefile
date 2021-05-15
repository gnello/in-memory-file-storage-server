ROOT=../

include $(ROOT)/.env
export

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

LIBS += -Wl,-rpath,$(ROOT)$(DATA_STRUCTURES_LIB) -L$(ROOT)$(DATA_STRUCTURES_LIB) -lgnl_list_t -lgnl_queue_t
INCLUDE += -I$(ROOT)$(DATA_STRUCTURES_INCLUDE)

TARGETS = main gnl_imfss_api.so
TARGETS_PATH = ./lib

.PHONY: all dev clean

all: $(TARGETS)
VPATH = src

%: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

%.so: %.o
	$(CC) -shared -o $(TARGETS_PATH)/lib$@ $(TARGETS_PATH)/$<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OPTFLAGS) -c -fPIC $< -o $(TARGETS_PATH)/$@

clean:
	cd $(TARGETS_PATH) && rm -f *.o *.so