#include "main.h"

/**
 * @brief support hexencoding a string
 * e.g 'A' 65:0100_0001 -> encoding -> 0000_0100(4), 0000_0001(1) -> 41
 * @param bin string to be hexencoded
 * @param len length of the input string
 * @return hexencoded string
 */
char *binary_to_hex(unsigned char *bin, size_t len)
{
    if (bin == NULL || len == 0)
        return NULL;

    char *out = malloc(len * 2 + 1);
    for (size_t i = 0; i < len; i++)
    {
        out[i * 2] = "0123456789ABCDEF"[bin[i] >> 4];
        out[i * 2 + 1] = "0123456789ABCDEF"[bin[i] & 0x0F];
    }
    out[len * 2] = '\0';

    return out;
}

/**
 * @brief support hexdecoding to hexcode to a char
 * e.g 61: 0000_0110, 0000_0001 -> decoding -> 0110_0001: 97 'a'
 * @param hex iuput char to be decoded
 * @param out output char has been decoded
 * @return 1 if successful decode else 0
 */
int hexchar_to_binary(const char hex, char *out)
{
    if (out == NULL)
        return 0;

    if (hex >= '0' && hex <= '9')
    {
        *out = hex - '0';
    }
    else if (hex >= 'A' && hex <= 'F')
    {
        *out = hex - 'A' + 10;
    }
    else if (hex >= 'a' && hex <= 'f')
    {
        *out = hex - 'a' + 10;
    }
    else
    {
        return 0;
    }

    return 1;
}

/**
 * @brief support hexdecoding hexcode to string
 * e.g. 676768 -> hexdecoding -> "ggh"
 * @param hex char array of the hex from
 * @param out pointer of char array of binary array
 * @return size_t the length of the binary array
 */
size_t hex_to_binary(const char *hex, unsigned char **out)
{
    size_t len;
    char b1;
    char b2;
    size_t i;

    if (hex == NULL || *hex == '\0' || out == NULL)
        return 0;

    len = strlen(hex);
    if (len % 2 != 0)
        return 0;
    len /= 2;

    *out = malloc(len);
    memset(*out, 'A', len);

    // out = *out;
    for (i = 0; i < len; i++)
    {
        if (!hexchar_to_binary(hex[i * 2], &b1) || !hexchar_to_binary(hex[i * 2 + 1], &b2))
        {
            return 0;
        }
        (*out)[i] = (b1 << 4) | b2;
    }

    return len;
}

/**
 * @brief      Substring a string
 *
 * @param      str  the string to be substringed
 * @param      substr  where the substringed string will be stored
 * @param[in]  start  the start index of the substring
 * @param[in]  end    the end index of the substring
 */
void substring(const char *str, char *substr, int start, int end)
{
    int i;
    for (i = start; i <= end; i++)
    {
        substr[i - start] = str[i];
    }
    substr[i - start] = '\0';
}

/**
 * @brief      Parse a string to an array
 *
 * @param      str  the string to be parsed, e.g.: "1,2,3,4" with seperator ','; "1/2/3/4" with seperator '/'
 * @param      array  where the parsed string will be stored
 * @param      sep  the separator of the string, e.g.: ',' or '/'
 * @param      size  the number of integers being parsed
 */
void parse_string_to_array(char *str, u_int32_t *array, char sep, int size)
{
    int start_pos[size + 1];
    int comma_count = 0;
    char *k_or_i[2] = {"-i", "-k"};
    start_pos[0] = 0;
    for (size_t j = 0; j < strlen(str); j++)
    {
        if (str[j] == sep)
        {
            start_pos[comma_count + 1] = j + 1;
            comma_count++;
        }
    }
    if (comma_count != (size - 1))
    {
        fprintf(stderr, "Error: %s must be %d integers separated by commas\n", k_or_i[size / 2 - 1], size);
        exit(EXIT_FAILURE);
    }
    start_pos[size] = strlen(str) + 1;
    for (int j = 0; j < size; j++)
    {
        char temp[start_pos[j + 1] - start_pos[j]];
        substring(str, temp, start_pos[j], start_pos[j + 1] - 2);
        // input check for integer
        for (size_t i = 0; i < strlen(temp); i++)
        {
            if (temp[i] < '0' || temp[i] > '9')
            {
                fprintf(stderr, "Error: %s must be %d integers separated by commas, non-integer character detected\n", k_or_i[size / 2 - 1], size);
                exit(EXIT_FAILURE);
            }
        }

        array[j] = (u_int32_t)atoi(temp);
    }
}
/**
 * @brief support function for parsing input arguments
 *
 * @param argc same as in main
 * @param argv same as in main
 * @return options_t struct of the parsed arguments
 */
options_t args_parser(int argc, char *argv[])
{

    u_int32_t imp_tmp = 0;
    u_int32_t ben_tmp = 0;
    u_int32_t deflag_tmp = 0;
    char *if_tmp = NULL;
    char *of_tmp = NULL;
    u_int32_t k_tmp[4] = {0, 0, 0, 0};
    u_int32_t i_tmp[2] = {0, 0};

    int filename_pos = 0;
    int wrong_sequence_flag = 0; // 0: no error, 1: wrong sequence
    for (int i = 1; i < argc; i++)
    {
        if (*argv[i] != '-')
        {
            if (filename_pos != 0)
            {
                fprintf(stderr, "Error: multiple input files\n");
                exit(EXIT_FAILURE);
            }
            if_tmp = argv[i];
            filename_pos = i;
        }
        else
        {
            if (strcmp(argv[i], "--help") == 0)
            {
                help(argv[0]);
                exit(EXIT_SUCCESS);
            }
            char *temp = malloc(sizeof(char) * 10);
            // char *temp = NULL;
            switch (*(argv[i] + 1))
            {
            case 'V':
                if (filename_pos != 0)
                {
                    fprintf(stderr, "Error: wrong sequence of args\n");
                    exit(EXIT_FAILURE);
                }
                // free(temp);
                // temp = malloc(sizeof(char)*(strlen(argv[i]) - 1));
                substring(argv[i], temp, 2, strlen(argv[i]) - 1);
                for (size_t i = 0; i < strlen(temp); i++)
                    if (temp[i] < '0' || temp[i] > MAX_IMP + '0')
                    {
                        fprintf(stderr, "Error: invalid implementation value, now we only support input from 0 to %d.\n", MAX_IMP);
                        exit(EXIT_FAILURE);
                    }
                imp_tmp = atoi(temp);
                if (imp_tmp > MAX_IMP)
                {
                    fprintf(stderr, "Error: invalid implementation value, now we only support input from 0 to %d.\n", MAX_IMP);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'B':
                if (filename_pos != 0)
                {
                    fprintf(stderr, "Error: wrong sequence of args\n");
                    exit(EXIT_FAILURE);
                }
                // free(temp);
                // temp = malloc(sizeof(char)*(strlen(argv[i]) - 1));
                substring(argv[i], temp, 2, strlen(argv[i]) - 1);
                for (size_t i = 0; i < strlen(temp); i++)
                    if (temp[i] < '0' || temp[i] > '9')
                    {
                        fprintf(stderr, "Error: invalid benchmark round value, it has to be an integer.\n");
                        exit(EXIT_FAILURE);
                    }
                ben_tmp = atoi(temp);
                break;
            case 'k':
                if (filename_pos == 0)
                    wrong_sequence_flag = 1;
                if ((i + 1) == argc || argv[i + 1][0] == '-')
                {
                    fprintf(stderr, "Error: not enough args for -k\n");
                    exit(EXIT_FAILURE);
                }
                i++;
                parse_string_to_array(argv[i], k_tmp, ',', 4);
                break;
            case 'i':
                if (filename_pos == 0)
                    wrong_sequence_flag = 1;
                if ((i + 1) == argc || argv[i + 1][0] == '-')
                {
                    fprintf(stderr, "Error: not enough args for -i\n");
                    exit(EXIT_FAILURE);
                }
                i++;
                parse_string_to_array(argv[i], i_tmp, ',', 2);
                break;
            case 'd':
                if (filename_pos == 0)
                    wrong_sequence_flag = 1;
                deflag_tmp = 1;
                break;
            case 'o':
                if (filename_pos == 0)
                    wrong_sequence_flag = 1;
                if ((i + 1) == argc || argv[i + 1][0] == '-')
                {
                    fprintf(stderr, "Error: not enough args for -o\n");
                    exit(EXIT_FAILURE);
                }
                i++;
                of_tmp = argv[i];
                break;
            case 'h':
                help(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Error: %s\n", "Unknown argument");
                exit(EXIT_FAILURE);
            }
            // free(temp);
        }
    }
    if (if_tmp == NULL)
    {
        fprintf(stderr, "Error: %s\n", "No input file specified");
        exit(EXIT_FAILURE);
    }
    if (wrong_sequence_flag == 1)
    {
        fprintf(stderr, "Error: %s\n", "Wrong sequence of args");
        exit(EXIT_FAILURE);
    }
    options_t options = {
        .key = {k_tmp[0], k_tmp[1], k_tmp[2], k_tmp[3]},
        .iv = {i_tmp[0], i_tmp[1]},
        .implementation = imp_tmp,
        .benchmark = ben_tmp,
        .decrypt_flag = deflag_tmp,
        .output_length = 0,
        .inputfile = if_tmp,
        .outputfile = of_tmp,
        .output_content = NULL};
    return options;
}

/**
 * @brief generate a new file name with _encrypted.txt or _decrypted.txt suffix
 *
 * @param options Options struct parsed from command line
 *
 * @return char* new file name
 */

char *filename_generator(options_t options)
{
    int point_pos = 0;
    for (int i = strlen(options.inputfile) - 1; i >= 0; i--)
        if (options.inputfile[i] == '.')
        {
            point_pos = i;
            break;
        }
    // e.g.: input.txt point_pos = 5, new_filename = input_encrypted.txt, new_len = 13
    char *new_filename = malloc(sizeof(char) * (point_pos + 14));
    substring(options.inputfile, new_filename, 0, point_pos - 1);
    if (options.decrypt_flag)
        strcat(new_filename, "_decrypted.txt");
    else
        strcat(new_filename, "_encrypted.txt");
    // fopen(new_filename, "rw");
    return new_filename;
}

/**
 * @brief      help function
 *
 * @param      argv  the argv
 */
void help(char *name)
{
    printf("Usage: %s %s\n", name, USAGE_FMT);
    printf("\n");
    printf("%s", HELP_MSG);
    printf("\n");
    // #define USAGE_EXAMPLE "Usage example: \n
    // ./main -V1 -B20 input.txt -k 1,2,3,4 -i 1,2 -d -o output.txt \n
    // ./main --help \n"
    printf("Usage example: \n\
    %s -V1 -B20 input.txt -k 1,2,3,4 -i 1,2 -d -o output.txt \n\
    %s --help \n",
           name, name);
}
