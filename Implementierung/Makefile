# Add additional compiler flags here
.PHONY: clean all
CC=gcc 
CFLAGS=-std=gnu11 -Wall -Wextra -Wpedantic -ggdb

all: main
testmain: main.c
	$(CC) $(CFLAGS) -o $@ $^
main: main.c 
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f main

