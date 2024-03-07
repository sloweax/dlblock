CC=cc
CFLAGS=-Wall -Wextra -Os -Wno-builtin-declaration-mismatch
BINDSTPATH=/usr/local/bin
LIBDSTPATH=/usr/local/lib

all: libdlblock.so dlblock

%.o: %.c
	$(CC) $(CFLAGS) -fPIC $^ -c -o $@

libdlblock.so: libdlblock.o
	$(CC) $(CFLAGS) -fPIC -shared $^ -o $@

dlblock: dlblock.o
	$(CC) $(CFLAGS) $^ -o $@

install: all
	mkdir -p $(BINDSTPATH)
	mkdir -p $(LIBDSTPATH)
	cp -f dlblock $(BINDSTPATH)
	cp -f libdlblock.so $(LIBDSTPATH)

uninstall:
	rm -f $(BINDSTPATH)/dlblock
	rm -f $(LIBDSTPATH)/libdlblock.so

clean:
	rm -f *.o *.so dlblock

.PHONY: clean all install uninstall
