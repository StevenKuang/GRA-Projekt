# Add additional compiler flags here
.PHONY: clean all
CC=gcc 
# if you don't wanna ignore unsigned char* diff from char* related warnings, delete -Wno-pointer-sign here
CFLAGS=-std=gnu11 -Wall -Wextra -Wpedantic -Wno-pointer-sign -ggdb -pg -Wno-unused-parameter -Wno-strict-aliasing -O2
DEPS=main.h

all: main
unittest: unittest.c xtea.c io.c utils.c benchmark.c xtea_decrypt_block_V1.S xtea_encrypt_block_V1.S 
	$(CC) $(CFLAGS) -o $@ $^
main: main.c xtea.c io.c utils.c benchmark.c xtea_decrypt_block_V1.S xtea_encrypt_block_V1.S
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f main unittest gmon.out

