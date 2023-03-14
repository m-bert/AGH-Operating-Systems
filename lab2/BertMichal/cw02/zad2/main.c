#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "reverse.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Too few arguments. Usage:\n");
        printf("./main <filename> <block_size>\n");

        return -1;
    }

    const char *input_path = argv[1];
    const int block_size = atoi(argv[2]);

    if (block_size <= 0)
    {
        printf("Block size has to be positive integer\n");

        return -1;
    }

    FILE *input_file = fopen(input_path, "r");

    if (!input_file)
    {
        printf("Failed to open input file\n");
        return -1;
    }

    const char *prefix = "rev_";
    const unsigned int output_path_len = strlen(input_path) + strlen(prefix);
    char output_path[output_path_len];
    sprintf(output_path, "%s%s", prefix, input_path);

    FILE *output_file = fopen(output_path, "w");

    if (!output_file)
    {
        printf("Failed to create output file\n");

        fclose(input_file);
        return -1;
    }

    const bool result = perform_test(input_path, block_size, input_file, output_file);

    if (!result)
    {
        return -1;
    }

    fclose(input_file);
    fclose(output_file);

    return 0;
}