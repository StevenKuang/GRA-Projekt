#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define USAGE_FMT "[-V] [-B] <file> [-k] [-i] [-d] [-o] [-h] [--help]"
#define HELP_MSG "options:\n\
\n\
    -V implementation<int>\n\
        Specifies which implementation will be used for the run. e.g.: -V0, -V1 etc...\n\
            0: C implementation\n\
            1: Assembly implementation\n\
            2: C implementation with look up table optimizations\n\
        (Default to executed using -V0)\n\
    -B benchmark<int>\n\
        If being set, the runtime of the run will be benchmarked and displayed. e.g.: -B1, -B20 etc...\n\
            <int>: (Optional) specifies how many time it benchmarks. (Default to 1 if not specified but -B being set).\n\
    -k key<int>\n\
        The key of the encryption/decryption, a 4 element tuple of decimal integers separated by commas. e.g.: 1,2,3,4 \n\
        (Default to 0,0,0,0)\n\
    -i initialization vector<int>\n\
        The initialization vector of the encryption/decryption, a 2 element tuple of decimal integers separated by commas. e.g.: 1,1 \n\
        (Default to 0,0)\n\
    -d decrypt\n\
        Decrypt the input file. If not specified, encryption will be performed.\n\
    -o outputfile\n\
        Specify the output file path. \n\
        If not specified, default file path would be: ./{input_path_stem}_{mode}.txt\n\
            mode: {encrypted, decrypted}\n\
            input_file_stem: The stem of the input file. e.g.: for \"input.txt\", it will be \"input\". \n\
        Example: \n\
            input_file: input.txt \n\
            mode: decryption \n\
            out: ./input_decrypted.txt\n\
    -h/--help \n\
        Print this help message. (Incompatible with other args.)\n"

#define DELTA 0x9E3779B9
#define NUM_ROUND 64
#define BLOCK_ALIGN 8
#define MAX_IMP 2
#define LAST_PADDING 0x808080808080808
#define SUM_INIT DELTA *NUM_ROUND

typedef struct
{
    u_int32_t const key[4];
    u_int32_t const iv[2];
    u_int32_t implementation;
    u_int32_t benchmark;
    u_int32_t decrypt_flag;
    size_t output_length;
    char *inputfile;
    char *outputfile;
    unsigned char *output_content;
} options_t;

// io.c
unsigned char *read_file(const char *path);
void write_file(const char *path, const unsigned char *string, size_t len);

// xtea.c
void xtea_encrypt_block(uint32_t v[2], uint32_t const key[4]);
void xtea_decrypt_block(uint32_t v[2], uint32_t const key[4]);
void xtea_encrypt_block_V2(uint32_t v[2], uint32_t const key[4]);
void xtea_decrypt_block_V2(uint32_t v[2], uint32_t const key[4]);
size_t padding(char *plain, size_t len);
void xtea_encrypt(uint32_t *plain, size_t len, uint32_t const key[4], uint32_t const iv[2], uint32_t imp);
void xtea_decrypt(uint32_t *cipher, size_t len, uint32_t const key[4], uint32_t const iv[2], uint32_t imp);
void build_lookup_table_for_encryption(uint32_t const key[4]);
void build_lookup_table_for_decryption(uint32_t const key[4]);

// utils.c
char *binary_to_hex(unsigned char *bin, size_t len);
int hexchar_to_binary(const char hex, char *out);
size_t hex_to_binary(const char *hex, unsigned char **out);
void substring(const char *str, char *substr, int start, int end);
void parse_string_to_array(char *str, u_int32_t *array, char sep, int size);
options_t args_parser(int argc, char *argv[]);
char *filename_generator(options_t options);
void help(char *name);

// benchmark.c
void xtea_decrypt_block_V1(uint32_t v[2], uint32_t const key[4]);
void xtea_encrypt_block_V1(uint32_t v[2], uint32_t const key[4]);
options_t benchmark(options_t options);
options_t run(options_t options);
