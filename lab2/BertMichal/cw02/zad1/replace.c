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
    int input_fd = open(input_path, O_RDONLY);

    if (input_fd == -1)
    {
        printf("Failed to open file\n");
        return false;
    }

    int output_fd = open(output_path, O_WRONLY | O_CREAT);

    if (output_fd == -1)
    {
        printf("Failed to create output file\n");
        return false;
    }

    chmod(output_path, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

    char *buffer = malloc(sizeof(char));

    while (read(input_fd, buffer, sizeof(char)) != 0)
    {
        if (*buffer == to_replace)
        {
            char *tmp_buffer = malloc(sizeof(char));
            *tmp_buffer = to_replace_with;

            write(output_fd, tmp_buffer, sizeof(char));
        }
        else
        {
            write(output_fd, buffer, sizeof(char));
        }
    }

    free(buffer);

    close(input_fd);
    close(output_fd);

    return true;
}

bool replace_C_lib(const char to_replace, const char to_replace_with, const char *input_path, const char *output_path)
{
    FILE *input_file = fopen(input_path, "r");

    if (!input_file)
    {
        printf("Failed to open file\n");
        return false;
    }

    FILE *output_file = fopen(output_path, "w");

    if (!output_file)
    {
        printf("Failed to create output file\n");
        return false;
    }

    char *buffer = malloc(sizeof(char));

    while (fread(buffer, sizeof(char), sizeof(char), input_file) != 0)
    {
        if (*buffer == to_replace)
        {
            char *tmp_buffer = malloc(sizeof(char));
            *tmp_buffer = to_replace_with;

            fwrite(tmp_buffer, sizeof(char), sizeof(char), output_file);
        }
        else
        {
            fwrite(buffer, sizeof(char), sizeof(char), output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);

    return true;
}