CC=cc
CSTD=c99
CFLAGS=-Wall -pedantic -std=$(CSTD) -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -Werror=implicit-function-declaration

all: pipesim

%.o: %.c
	$(CC) $(CFLAGS) -c $<

pipesim: pipesim.o
	$(CC) -o $@ $^
