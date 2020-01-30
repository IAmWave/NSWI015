CC=cc
CSTD=c99
CFLAGS=-Wall -pedantic -std=$(CSTD) -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -Werror=implicit-function-declaration

all: inetd

%.o: %.c
	$(CC) $(CFLAGS) -c $<

inetd: inetd.o
	$(CC) -o $@ $^
