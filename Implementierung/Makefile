# Add additional compiler flags here
.PHONY: clean all
CC=gcc 
CFLAGS=-std=gnu11 -Wall -Wextra -Wpedantic -ggdb

all: main
testassembly: main.c decipherOptimized.S encipherOptimized.S
	$(CC) $(CFLAGS) -o $@ $^
main: main.c 
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f main

