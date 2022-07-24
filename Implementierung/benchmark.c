
#include <time.h>
#include "main.h"

options_t benchmark(options_t options)
{
    struct timespec start, end, p_start, p_end; // one for the entire process, one for purely the encryption/decryption
    long res_sec = 0;
    long res_nano = 0;
    long p_res_nano = 0;
    long p_res_sec = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    unsigned char *input_str = read_file(options.inputfile);
    int input_byte_len = strlen(input_str);
    for (u_int32_t i = 0; i < options.benchmark; i++)
    {
        if (options.decrypt_flag)
        { // then input_str is a hex string
            free(options.output_content);
            unsigned char *input_str_binary;
            input_byte_len = hex_to_binary(input_str, &input_str_binary);
            clock_gettime(CLOCK_MONOTONIC, &p_start);
            xtea_decrypt((uint32_t *)input_str_binary, input_byte_len, options.key, options.iv, options.implementation);
            clock_gettime(CLOCK_MONOTONIC, &p_end);
            p_res_nano += p_end.tv_nsec - p_start.tv_nsec;
            p_res_sec += p_end.tv_sec - p_start.tv_sec;
            p_res_nano += (p_end.tv_sec == p_start.tv_sec) ? p_end.tv_nsec - p_start.tv_nsec : p_end.tv_nsec + 1000000000 - p_start.tv_nsec;
            if (p_res_nano > 1000000000)
            {
                p_res_sec += 1;
                p_res_nano -= 1000000000;
            }
            size_t pad_num = input_str_binary[input_byte_len - 1];
            if (pad_num < 1 || pad_num > 8)
            {
                fprintf(stderr, "Error: Possible false decryption key or iv.\n");
                exit(EXIT_FAILURE);
            }
            for (size_t i = 1; i < pad_num; i++)
                if (input_str_binary[input_byte_len - i] != pad_num)
                {
                    fprintf(stderr, "Error: Possible false decryption key or iv.\n");
                    exit(EXIT_FAILURE);
                }
            options.output_length = input_byte_len - pad_num;
            options.output_content = input_str_binary;
        }
        else
        {
            // make a copy so the same data will be encrypted each time
            free(options.output_content);
            unsigned char *input_str_cpy = (unsigned char *)malloc(input_byte_len);
            strcpy(input_str_cpy, input_str);

            clock_gettime(CLOCK_MONOTONIC, &p_start);
            xtea_encrypt((uint32_t *)input_str_cpy, input_byte_len, options.key, options.iv, options.implementation);
            clock_gettime(CLOCK_MONOTONIC, &p_end);
            p_res_nano += (p_end.tv_sec == p_start.tv_sec) ? p_end.tv_nsec - p_start.tv_nsec : p_end.tv_nsec + 1000000000 - p_start.tv_nsec;
            if (p_res_nano > 1000000000)
            {
                p_res_sec += 1;
                p_res_nano -= 1000000000;
            }
            int padded_byte_len = 8 * ((int)input_byte_len / 8 + 1);
            options.output_length = 2 * padded_byte_len;
            options.output_content = binary_to_hex(input_str_cpy, options.output_length);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    res_sec = end.tv_sec - start.tv_sec;
    if (end.tv_nsec > start.tv_nsec)
        res_nano = end.tv_nsec - start.tv_nsec;
    else
    {
        res_sec--;
        res_nano = end.tv_nsec + 1000000000 - start.tv_nsec;
    }
    printf("Total runtime for running %d times: \t\t\t\t%ld seconds and %ld nanoseconds\n", options.benchmark, res_sec, res_nano);
    long avg_runtime_nano = 1000000000 * ((float)res_sec / options.benchmark - (long)res_sec / options.benchmark) + (long)res_nano / options.benchmark;
    printf("Average runtime for running %d times: \t\t\t\t%ld seconds and %ld nanoseconds\n", options.benchmark, (long)res_sec / options.benchmark, avg_runtime_nano);
    printf("Total time for %d times encryption/decryption: \t\t%ld seconds and %ld nanoseconds\n", options.benchmark, p_res_sec, p_res_nano);
    long avg_time_nano = 1000000000 * ((float)p_res_sec / options.benchmark - (long)p_res_sec / options.benchmark) + (long)p_res_nano / options.benchmark;
    printf("Average time for %d times encryption/decryption: \t\t%ld seconds and %ld nanoseconds\n", options.benchmark, (long)p_res_sec / options.benchmark, avg_time_nano);
    return options;
}

options_t run(options_t options)
{
    // start reading input file
    unsigned char *input_str = read_file(options.inputfile);
    int input_byte_len = strlen(input_str);
    if (options.decrypt_flag)
    { 
        // then input_str is a hex string
        unsigned char *input_str_binary;
        input_byte_len = hex_to_binary(input_str, &input_str_binary);
        xtea_decrypt((uint32_t *)input_str_binary, input_byte_len, options.key, options.iv, options.implementation);
        size_t pad_num = input_str_binary[input_byte_len - 1];
        if (pad_num < 1 || pad_num > 8)
        {
            fprintf(stderr, "Error: Possible false decryption key or iv.\n");
            exit(EXIT_FAILURE);
        }
        for (size_t i = 1; i < pad_num; i++)
            if (input_str_binary[input_byte_len - i] != pad_num)
            {
                fprintf(stderr, "Error: Possible false decryption key or iv.\n");
                exit(EXIT_FAILURE);
            }
        options.output_length = input_byte_len - pad_num;
        options.output_content = input_str_binary;
        printf("DECRYPT SUCCESS\n");
    }
    else
    {
        xtea_encrypt((uint32_t *)input_str, input_byte_len, options.key, options.iv, options.implementation);
        int padded_byte_len = 8 * ((int)input_byte_len / 8 + 1);
        options.output_length = 2 * padded_byte_len;
        options.output_content = binary_to_hex(input_str, options.output_length);
        printf("ENCRYPT SUCCESS\n");
    }
    return options;
}