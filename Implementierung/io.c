#include "main.h"

/**
 * @brief read the file from the path
 *
 * @param path the path of the file
 * @return char* pointer of the file
 */
unsigned char *read_file(const char *path)
{
    unsigned char *string = NULL;
    FILE *file;
    // file open failed
    if (!(file = fopen(path, "r")))
    {
        errno = EIO;
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // store the file status in statbuf
    struct stat statbuf;
    if (fstat(fileno(file), &statbuf))
    {
        errno = EIO;
        perror("Error retrieving file stats");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // if is not the regular file or file has invalid size
    if (!S_ISREG(statbuf.st_mode) || statbuf.st_size <= 0)
    {
        errno = EIO;
        perror("Error: Not a regular file or invalid size");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int padded_st_size = 8 * ((int)statbuf.st_size / 8 + 1);
    if (!(string = malloc(padded_st_size + 1)))
    {
        errno = EIO;
        perror("Error: allocating memeory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (!fread(string, 1, statbuf.st_size, file))
    {
        errno = EIO;
        perror("Error reading file");
        free(string);
        string = NULL;
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return string;
}

/**
 * @brief write the content into the file
 *
 * @param path the path of the file  that be written
 * @param string the context writes into the file
 */
void write_file(const char *path, const unsigned char *string, size_t len)
{
    FILE *file;

    if (!(file = fopen(path, "w")))
    {
        errno = EIO;
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (!fwrite(string, 1, len, file))
    {
        errno = EIO;
        perror("Error writting file"); // Error writing file: Success
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return;
}