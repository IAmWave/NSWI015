CFLAGS=-Wall -Wextra -std=c99 -g

build/:
	mkdir -p build

build/%: build/
	$(CC) $(CFLAGS) -o $@ $*.c
