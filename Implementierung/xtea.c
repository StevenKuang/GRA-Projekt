#include "main.h"

uint32_t *lookup_table_1 = NULL;
uint32_t *lookup_table_2 = NULL;

void (*encrypt_block_versions[MAX_IMP + 1])(uint32_t[], uint32_t const[]) = {xtea_encrypt_block, xtea_encrypt_block_V1, xtea_encrypt_block_V2};
void (*decrypt_block_versions[MAX_IMP + 1])(uint32_t[], uint32_t const[]) = {xtea_decrypt_block, xtea_decrypt_block_V1, xtea_decrypt_block_V2};
/**
 * Encrypt a block of 8 bytes using XTEA.
 * @param v two 32-bit input of plain text
 * @param key four 32-bit input of key
 */
void xtea_encrypt_block(uint32_t v[2], uint32_t const key[4])
{
    uint32_t v0 = v[0], v1 = v[1], sum = 0;
    for (size_t i = 0; i < NUM_ROUND; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += DELTA;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

/**
 * Decrypt a block of 8 bytes using XTEA.
 * @param v two 32-bit input of cipher text
 * @param key four 32-bit input of key
 */
void xtea_decrypt_block(uint32_t v[2], uint32_t const key[4])
{
    uint32_t v0 = v[0], v1 = v[1], sum = DELTA * NUM_ROUND; 
    for (size_t i = 0; i < NUM_ROUND; i++)
    {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
        sum -= DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

/**
 * Encrypt a block of 8 bytes using XTEA.
 * @param v two 32-bit input of plain text
 * @param key four 32-bit input of key
 */
void xtea_encrypt_block_V2(uint32_t v[2], uint32_t const key[4])
{
    uint32_t v0 = v[0], v1 = v[1];
    for (size_t i = 0; i < NUM_ROUND; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ lookup_table_1[i];
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ lookup_table_2[i];
    }
    v[0] = v0;
    v[1] = v1;
}

/**
 * Decrypt a block of 8 bytes using XTEA.
 * @param v two 32-bit input of cipher text
 * @param key four 32-bit input of key
 */
void xtea_decrypt_block_V2(uint32_t v[2], uint32_t const key[4])
{
    uint32_t v0 = v[0], v1 = v[1];
    for (size_t i = 0; i < NUM_ROUND; i++)
    {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ lookup_table_1[i];
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ lookup_table_2[i];
    }
    v[0] = v0;
    v[1] = v1;
}

/**
 * Pad the input of plain text to make its length multiple of the block align (8 bytes).
 * If the length of plain text is already multiple of the block align, another block will be added.
 * @param v pointer of the plain text
 * @param len length of the plain text before padding
 * @return length of the plain text after padding
 */
size_t padding(char *plain, size_t len)
{
    size_t num_padding = 8 - len % 8;
    for (size_t i = 0; i < num_padding; i++)
        plain[len + i] = (char)(num_padding);
    return len + num_padding;
}

/**
 * Encrypt blocks of plain text.
 * @param plain plain text
 * @param len length of the plain text
 * @param key input of key
 * @param iv initial vector
 */
void xtea_encrypt(uint32_t *plain, size_t len, uint32_t const key[4], uint32_t const iv[2], u_int32_t imp)
{
    // pad the plain text
    len = padding((char *)plain, len);
    if (imp == 2) build_lookup_table_for_encryption(key);
    uint32_t v[2] = {iv[0], iv[1]};
    for (size_t i = 0; i < len / 4; i += 2)
    {
        // xor with the former vector
        v[0] = plain[i] ^ v[0];
        v[1] = plain[i + 1] ^ v[1];
        // encrypt the block
        encrypt_block_versions[imp](v, key);
        // write back to memory
        plain[i] = v[0];
        plain[i + 1] = v[1];
    }
    if (imp == 2) 
    {
        free(lookup_table_1);
        free(lookup_table_2);
    }
}

/**
 * Decrypt blocks of cipher text.
 * @param cipher cipher text
 * @param len length of the cipher text
 * @param key input of key
 * @param iv initial vector
 */

void xtea_decrypt(uint32_t *cipher, size_t len, uint32_t const key[4], uint32_t const iv[2], uint32_t imp)
{
    if (len % 8 != 0)
    {
        errno = EINVAL;
        perror("Invalid cipher length, should be multiple of block size 8");
        exit(EXIT_FAILURE);
    }
    if (imp == 2) build_lookup_table_for_decryption(key);

    // convert 32 bit cipher text to 64 bit cipher text, to accelerate the decryption
    uint64_t *cipher_64 = (uint64_t *)cipher;
    uint64_t tmp1 = *(uint64_t *)iv;
    uint64_t tmp2 = 0;
    for (size_t i = 0; i < len / 8; i++)
    {
        // save the block for the next round
        tmp2 = cipher_64[i];
        uint32_t v[] = {(uint32_t)(cipher_64[i] & 0xFFFFFFFF), (uint32_t)(cipher_64[i] >> 32)};
        // decrypt the block
        decrypt_block_versions[imp](v, key);
        // xor with the former vector
        cipher_64[i] = *(uint64_t *)v ^ tmp1;
        // update the vector for next round
        tmp1 = tmp2;
    }
    if (imp == 2) 
    {
        free(lookup_table_1);
        free(lookup_table_2);
    }
}
/**
 * Build lookup table for encryption.
 * @param key input of key
 */
void build_lookup_table_for_encryption(uint32_t const key[4])
{
    // if (!(lookup_table_1 = malloc(sizeof(*lookup_table_1) * 8)) || !(lookup_table_2 = malloc(sizeof(*lookup_table_2) * 8)))
    if (!(lookup_table_1 = malloc(sizeof(uint32_t) * 64)) || !(lookup_table_2 = malloc(sizeof(uint32_t) * 64)))
    {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    uint32_t sum = 0;
    for (size_t i = 0; i < NUM_ROUND; i++)
    {
        lookup_table_1[i] = sum + key[sum & 3];
        sum += DELTA;
        lookup_table_2[i] = sum + key[(sum >> 11) & 3];
    }
}
/**
 * Build lookup table for decryption.
 * @param key input of key
 */
void build_lookup_table_for_decryption(uint32_t const key[4])
{
    // if (!(lookup_table_1 = malloc(sizeof(*lookup_table_1) * 8)) || !(lookup_table_2 = malloc(sizeof(*lookup_table_2) * 8)))
    if (!(lookup_table_1 = malloc(sizeof(uint32_t) * 64)) || !(lookup_table_2 = malloc(sizeof(uint32_t) * 64)))
    {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    uint32_t sum = SUM_INIT;
    for (size_t i = 0; i < NUM_ROUND; i++)
    {
        lookup_table_1[i] = sum + key[(sum >> 11) & 3];
        sum -= DELTA;
        lookup_table_2[i] = sum + key[sum & 3];
    }
}