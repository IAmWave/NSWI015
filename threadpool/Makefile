CC=cc
CSTD=c99
CFLAGS=-Wall -pedantic -std=$(CSTD) -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -Werror=implicit-function-declaration

all: threadpool

%.o: %.c
	$(CC) $(CFLAGS) -c $<

threadpool: threadpool.o
	$(CC) -o $@ $^
