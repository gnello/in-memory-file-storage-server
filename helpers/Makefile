CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

TARGETS = gnl_colorshell.so gnl_assert.so
TARGETSPATH = ./../dist/helpers/lib
REALTARGETSPATH = $(realpath $(TARGETSPATH))

.PHONY: all clean copyheaders

all: $(TARGETS)
VPATH = src

%.so: %.o copyheaders
	$(CC) -shared -o $(REALTARGETSPATH)/lib$@ $(REALTARGETSPATH)/$<.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -fPIC $< -o $(REALTARGETSPATH)/$@.o

copyheaders:
	cp includes/* $(REALTARGETSPATH)/../includes

clean:
	-rm -f $(REALTARGETSPATH)/*.o $(REALTARGETSPATH)/*.so $(REALTARGETSPATH)/../includes/*.h