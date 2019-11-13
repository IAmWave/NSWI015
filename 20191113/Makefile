CFLAGS=-Wall -Wextra -std=c99 -g

build/:
	mkdir -p build

build/%: build/ %.c
	$(CC) $(CFLAGS) -o $@ $*.c

#test: test.sh build/palindrome data/*
#	@./test.sh
