#include "main.h"
#include "assert.h"

uint32_t *lookup_table_1;
uint32_t *lookup_table_2;

// function names are quite self-explanatory.
void args_parser_test()
{
    // prepare test data
    u_int32_t const k[4] = {12, 23, 34, 45};
    u_int32_t const iv[2] = {1, 2};
    char *argv[] = {"./test", "-V1", "-B200", "input.txt", "-o", "output.txt", "-k", "12,23,34,45", "-i", "1,2", "-d"};
    // run test
    options_t options = args_parser(11, argv);
    // check result
    assert(options.decrypt_flag == 1);
    assert(strcmp(options.inputfile, "input.txt") == 0);
    assert(strcmp(options.outputfile, "output.txt") == 0);
    assert(options.implementation == 1);
    assert(options.benchmark == 200);
    for (size_t i = 0; i < 4; i++)
        assert(options.key[i] == k[i]);
    for (size_t j = 0; j < 2; j++)
        assert(options.iv[j] == iv[j]);
    printf("Argument parsing correct\n");
    return;
}

void read_file_test()
{
    // prepare test data
    char *path = "./test/input.txt";
    unsigned char *expected = "I think we have done it. read";
    // run test
    unsigned char *result = read_file(path);
    // check result
    // size_t res_len = strlen((char*)result);
    assert(strcmp((char *)result, (char *)expected) == 0);
    free(result);
    printf("File reading correct\n");
    return;
}

void write_file_test()
{
    char *path = "./test/output.txt";
    unsigned char *string = "I think we have done it. again. eof";
    size_t len = strlen((unsigned char *)string);
    // run test
    write_file(path, string, len);
    // check result
    unsigned char *result = malloc(len);
    result = read_file(path);
    assert(strcmp((char *)result, (char *)string) == 0);
    free(result);
    printf("File writing correct\n");
    return;
}

void xtea_encrypt_block_test()
{
    // prepare test data
    uint32_t v[2] = {0x12345678, 0x9abcdef0};
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    // run test
    xtea_encrypt_block(v, key);
    // check result
    assert(v[0] == 0xdbfbf1d8);
    assert(v[1] == 0xca1c20b);
    printf("XTEA block encryption V0 correct\n");

    v[0] = 0x12345678;
    v[1] = 0x9abcdef0;
    xtea_encrypt_block_V1(v, key);
    assert(v[0] == 0xdbfbf1d8);
    assert(v[1] == 0xca1c20b);
    printf("XTEA block encryption V1 correct\n");

    v[0] = 0x12345678;
    v[1] = 0x9abcdef0;
    build_lookup_table_for_encryption(key);
    xtea_encrypt_block_V2(v, key);
    assert(v[0] == 0xdbfbf1d8);
    assert(v[1] == 0xca1c20b);
    printf("XTEA block encryption V2 correct\n");
    return;
}

void xtea_encrypt_block_test_max()
{
    // prepare test data
    uint32_t v[2] = {0xffffffff, 0xffffffff};
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    size_t count = 0;
    // run test
    xtea_encrypt_block(v, key);
    // check result
    assert(v[0] == 0xd1db4b40);
    assert(v[1] == 0x58af8856);
    count++;

    v[0] = 0xffffffff;
    v[1] = 0xffffffff;
    
    xtea_encrypt_block_V1(v, key);
    assert(v[0] == 0xd1db4b40);
    assert(v[1] == 0x58af8856);
    count++;

    v[0] = 0xffffffff;
    v[1] = 0xffffffff;
    build_lookup_table_for_encryption(key);
    xtea_encrypt_block_V2(v, key);
    assert(v[0] == 0xd1db4b40);
    assert(v[1] == 0x58af8856);
    count++;
    if (count == 3)
        printf("XTEA block encryption V0-V2 max edge case correct\n");
    else
        printf("XTEA block encryption V0-V2 max edge case incorrect\n");
    return;
}

void xtea_encrypt_block_test_min()
{
    // prepare test data
    uint32_t v[2] = {0x00000000, 0x00000000};
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    size_t count = 0;
    // run test
    xtea_encrypt_block(v, key);
    // check result
    assert(v[0] == 0x25522DF8);
    assert(v[1] == 0xEA4E3BCC);
    count++;

    v[0] = 0x00000000;
    v[1] = 0x00000000;
    xtea_encrypt_block_V1(v, key);
    assert(v[0] == 0x25522DF8);
    assert(v[1] == 0xEA4E3BCC);
    count++;

    v[0] = 0x00000000;
    v[1] = 0x00000000;
    build_lookup_table_for_encryption(key);
    xtea_encrypt_block_V2(v, key);
    assert(v[0] == 0x25522DF8);
    assert(v[1] == 0xEA4E3BCC);
    count++;
    if (count == 3)
        printf("XTEA block encryption V0-V2 min edge case correct\n");
    else
        printf("XTEA block encryption V0-V2 min edge case incorrect\n");
    return;
}

void xtea_decrypt_block_test()
{
    // prepare test data
    uint32_t v[2] = {0xdbfbf1d8, 0xca1c20b};
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    // run test
    xtea_decrypt_block(v, key);
    // check result
    assert(v[0] == 0x12345678);
    assert(v[1] == 0x9abcdef0);
    printf("XTEA block decryption V0 correct\n");

    v[0] = 0xdbfbf1d8;
    v[1] = 0xca1c20b;
    xtea_decrypt_block_V1(v, key);
    assert(v[0] == 0x12345678);
    assert(v[1] == 0x9abcdef0);
    printf("XTEA block decryption V1 correct\n");

    v[0] = 0xdbfbf1d8;
    v[1] = 0xca1c20b;
    build_lookup_table_for_decryption(key);
    xtea_decrypt_block_V2(v, key);
    assert(v[0] == 0x12345678);
    assert(v[1] == 0x9abcdef0);
    printf("XTEA block decryption V2 correct\n");
    return;
}

void xtea_decrypt_block_test_max()
{
    // prepare test data
    uint32_t v[2] = {0xffffffff, 0xffffffff};
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    size_t count = 0;
    // run test
    xtea_decrypt_block(v, key);
    // check result
    assert(v[0] == 0x46E6BA4B);
    assert(v[1] == 0xE51C7A43);
    count++;

    v[0] = 0xffffffff;
    v[1] = 0xffffffff;
    xtea_decrypt_block_V1(v, key);
    assert(v[0] == 0x46E6BA4B);
    assert(v[1] == 0xE51C7A43);
    count++;

    v[0] = 0xffffffff;
    v[1] = 0xffffffff;
    build_lookup_table_for_decryption(key);
    xtea_decrypt_block_V2(v, key);
    assert(v[0] == 0x46E6BA4B);
    assert(v[1] == 0xE51C7A43);
    count++;
    if (count == 3)
        printf("XTEA block decryption V0-V2 max edge case correct\n");
    else
        printf("XTEA block decryption V0-V2 max edge case incorrect\n");
    return;
}

void xtea_decrypt_block_test_min()
{
    // prepare test data
    uint32_t v[2] = {0x00000000, 0x00000000};
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    size_t count = 0;
    // run test
    xtea_decrypt_block(v, key);
    // check result
    assert(v[0] == 0xDA18AD71);
    assert(v[1] == 0x79FED280);
    count++;

    v[0] = 0x00000000;
    v[1] = 0x00000000;
    xtea_decrypt_block_V1(v, key);
    assert(v[0] == 0xDA18AD71);
    assert(v[1] == 0x79FED280);
    count++;

    v[0] = 0x00000000;
    v[1] = 0x00000000;
    build_lookup_table_for_decryption(key);
    xtea_decrypt_block_V2(v, key);
    assert(v[0] == 0xDA18AD71);
    assert(v[1] == 0x79FED280);
    count++;
    if (count == 3)
        printf("XTEA block decryption V0-V2 max edge case correct\n");
    else
        printf("XTEA block decryption V0-V2 max edge case incorrect\n");
    return;
}

void binary_to_hex_test()
{
    // prepare test data
    unsigned char input[4] = {0x12, 0x34, 0x56, 0x78};
    char const expected[8] = "12345678";
    // run test
    char *result = binary_to_hex(input, 4);
    // check result
    assert(strcmp(result, expected) == 0);
    free(result);
    printf("Binary to hex correct\n");
    return;
}
void binary_to_hex_test_max()
{
    // prepare test data
    unsigned char input[4] = {0xff, 0xff, 0xff, 0xff};
    char const expected[8] = "FFFFFFFF";
    // run test
    char *result = binary_to_hex(input, 4);
    // check result
    assert(strcmp(result, expected) == 0);
    free(result);
    printf("Binary to hex max case correct\n");
    return;
}
void binary_to_hex_test_min()
{
    // prepare test data
    unsigned char input[4] = {0x00, 0x00, 0x00, 0x00};
    char const expected[8] = "00000000";
    // run test
    char *result = binary_to_hex(input, 4);
    // check result
    assert(strcmp(result, expected) == 0);
    free(result);
    printf("Binary to hex min case correct\n");
    return;
}

void hex_to_binary_test()
{
    // prepare test data
    char const input[8] = "12345678";
    unsigned char expected[4] = {0x12, 0x34, 0x56, 0x78};
    unsigned char *result = malloc(4);
    // run test
    hex_to_binary(input, &result);
    // check result
    assert(memcmp(result, expected, 4) == 0);
    free(result);
    printf("Hex to binary correct\n");
    return;
}
void hex_to_binary_test_max()
{
    // prepare test data
    char const input[8] = "FFFFFFFF";
    unsigned char expected[4] = {0xff, 0xff, 0xff, 0xff};
    unsigned char *result = malloc(4);
    // run test
    hex_to_binary(input, &result);
    // check result
    assert(memcmp(result, expected, 4) == 0);
    free(result);
    printf("Hex to binary max case correct\n");
    return;
}
void hex_to_binary_test_min()
{
    // prepare test data
    char const input[8] = "00000000";
    unsigned char expected[4] = {0x00, 0x00, 0x00, 0x00};
    unsigned char *result = malloc(4);
    // run test
    hex_to_binary(input, &result);
    // check result
    assert(memcmp(result, expected, 4) == 0);
    free(result);
    printf("Hex to binary min case correct\n");
    return;
}

void xtea_encrypt_test_less_than_8()
{
    // prepare test data
    unsigned char *input_str = malloc(9);
    strcpy(input_str, "12345");
    uint32_t *input_binary = (uint32_t *)input_str;
    size_t input_len = 5;
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    uint32_t const iv[2] = {0x12345678, 0x9abcdef0};
    uint32_t const expected[2] = {2996686796, 3746346594};
    // run test
    xtea_encrypt(input_binary, input_len, key, iv, 0);
    // check result
    assert(memcmp(input_binary, expected, 8) == 0);
    free(input_str);
    printf("XTEA encryption with input's length < block length (padding required) correct\n");
    return;
}
void xtea_encrypt_test_exactly_8()
{
    // prepare test data
    unsigned char *input_str = malloc(9);
    strcpy(input_str, "12345678");
    uint32_t *input_binary = (uint32_t *)input_str;
    size_t input_len = 8;
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    uint32_t const iv[2] = {0x12345678, 0x9abcdef0};
    uint32_t const expected[2] = {527399618, 1888312437};
    // run test
    xtea_encrypt(input_binary, input_len, key, iv, 0);
    // check result
    assert(memcmp(input_binary, expected, 8) == 0);
    free(input_str);
    printf("XTEA encryption with input's length == block length (padding not required) correct\n");
    return;
}

void xtea_decrypt_test_less_than_8()
{
    // prepare test data
    uint32_t input_binary[2] = {2996686796, 3746346594};
    size_t input_len_padded = 8;
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    uint32_t const iv[2] = {0x12345678, 0x9abcdef0};
    uint32_t const expected[2] = {875770417, 50529077}; // binary of "12345\003\003\003"
    // run test
    xtea_decrypt(input_binary, input_len_padded, key, iv, 0);
    // check result
    assert(memcmp(input_binary, expected, 8) == 0);
    printf("XTEA decryption with input's length < block length (padding required) correct\n");
    return;
}

void xtea_decrypt_test_exactly_8()
{
    // prepare test data
    uint32_t input_binary[2] = {527399618, 1888312437};
    size_t input_len_padded = 8;
    uint32_t const key[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
    uint32_t const iv[2] = {0x12345678, 0x9abcdef0};
    uint32_t const expected[2] = {875770417, 943142453}; // binary of "12345678"
    // run test
    xtea_decrypt(input_binary, input_len_padded, key, iv, 0);
    // check result
    assert(memcmp(input_binary, expected, 8) == 0);
    printf("XTEA decryption with input's length == block length (padding not required) correct\n");
    return;
}

int main()
{
    printf("Welcome to the Unit Test of Project Team 109 -- XTEA En/Decryption\n");
    printf("The unit test will test the following functions:\n");
    printf("args_parser, read_file, write_file, xtea_encrypt, xtea_decrypt, xtea_encrypt_block, xtea_decrypt_block, binary_to_hex, hex_to_binary\n");
    printf("If any of the tests fails, the program will exit with an error thrown by the corresponding assert function.\n");
    printf("Please wait...\n\n");

    

    args_parser_test();
    read_file_test();
    write_file_test();
    xtea_encrypt_block_test();
    xtea_encrypt_block_test_max();
    xtea_encrypt_block_test_min();
    xtea_decrypt_block_test();
    xtea_decrypt_block_test_max();
    xtea_decrypt_block_test_min();
    binary_to_hex_test();
    binary_to_hex_test_max();
    binary_to_hex_test_min();
    hex_to_binary_test();
    hex_to_binary_test_max();
    hex_to_binary_test_min();
    xtea_encrypt_test_less_than_8();
    xtea_encrypt_test_exactly_8();
    xtea_decrypt_test_less_than_8();
    xtea_decrypt_test_exactly_8();
    printf("\nAll tests passed!\n");
    return 0;
}