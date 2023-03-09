#include "replace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

bool validate_parameters(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Wrong number of parameters\n");
        printf("Usage:\n./main <char_to_be_replaced> <char_to_replace_with> <input_file> <output_file>\n");

        return false;
    }

    if (strlen(argv[1]) != 1)
    {
        printf("Char to be replaced has to be single character\n");
        return false;
    }

    if (strlen(argv[2]) != 1)
    {
        printf("Char to replace with has to be single character\n");
        return false;
    }

    return true;
}

bool replace_system_lib(const char to_replace, const char to_replace_with, const char *input_path, const char *output_path)
{
    // Open input file
    int input_fd = open(input_path, O_RDONLY);

    if (input_fd == -1)
    {
        printf("Failed to open file\n");
        return false;
    }

    // Create output file
    int output_fd = open(output_path, O_WRONLY | O_CREAT | O_EXCL);

    if (output_fd == -1)
    {
        printf("Failed to create (file may already exist)\n");
        return false;
    }

    return true;
}

bool replace_C_lib(const char to_replace, const char to_replace_with, const char *input_path, const char *output_path)
{
    return true;
}