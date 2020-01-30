CC=cc
CSTD=c99
CFLAGS=-Wall -pedantic -std=$(CSTD) -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -Werror=implicit-function-declaration

all: server client

%.o: %.c
	$(CC) $(CFLAGS) -c $<

server: server.o
	$(CC) -o $@ $^

client: client.o
	$(CC) -o $@ $^
