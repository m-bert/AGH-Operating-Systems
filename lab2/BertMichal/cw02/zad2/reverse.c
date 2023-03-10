#include "reverse.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

bool reverse_file(const char *input_path, const int block_size, FILE *input_file, FILE *output_file)
{
    char *buffer = malloc(sizeof(char) * block_size);

    // Go to end of file
    int current_position = 0;
    fseek(input_file, -block_size * sizeof(char), SEEK_END);

    // This loops runs while we can read whole block of data
    while (ftell(input_file) != current_position)
    {
        current_position = ftell(input_file);

        fread(buffer, sizeof(char), block_size * sizeof(char), input_file);
        reverse_str(buffer);
        fwrite(buffer, sizeof(char), block_size * sizeof(char), output_file);

        fseek(input_file, -block_size * sizeof(char), SEEK_CUR); // Returns to current_position
        fseek(input_file, -block_size * sizeof(char), SEEK_CUR);
    }

    // Since while loop reads only full blocks, we have to read remaing part of the file
    // which length doesn't have to be a multiple of block_size
    int remaining_bytes = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    if (block_size == 1)
    {
        free(buffer);
        return true;
    }

    if (remaining_bytes == 0)
    {
        remaining_bytes = fread(buffer, sizeof(char), block_size * sizeof(char), input_file);
        reverse_str(buffer);
        fwrite(buffer, sizeof(char), remaining_bytes * sizeof(char), output_file);
    }
    else
    {
        char *tmp_buffer = malloc(sizeof(char) * remaining_bytes);
        fread(tmp_buffer, sizeof(char), remaining_bytes * sizeof(char), input_file);
        reverse_str(tmp_buffer);
        fwrite(tmp_buffer, sizeof(char), remaining_bytes * sizeof(char), output_file);

        free(tmp_buffer);
    }

    free(buffer);

    return true;
}

void reverse_str(char *str)
{
    const unsigned int n = strlen(str);
    char tmp;

    for (int i = 0; i < n / 2; ++i)
    {
        tmp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = tmp;
    }
}

bool perform_test(const char *input_path, const int block_size, FILE *input_file, FILE *output_file)
{
    clock_t block_start_time = clock();
    const bool reverse_by_block_succeed = reverse_file(input_path, block_size, input_file, output_file);
    clock_t block_end_time = clock();

    if (!reverse_by_block_succeed)
    {
        return false;
    }

    clock_t block_total_time = block_end_time - block_start_time;
    double block_total_ms = block_total_time / (double)CLOCKS_PER_SEC * 1000;

    printf("Reversed file with with block_size = %d in %.5f ms\n", block_size, block_total_ms);

    return true;
}