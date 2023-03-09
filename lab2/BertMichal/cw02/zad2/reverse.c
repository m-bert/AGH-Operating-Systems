#include "reverse.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

bool reverse_file(const char *input_path, const int block_size)
{
    FILE *input_file = fopen(input_path, "r");

    if (!input_file)
    {
        printf("Failed to open input file\n");
        return false;
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
        return false;
    }

    fclose(input_file);
    fclose(output_file);
}

void reverse_str(char *str)
{
    const unsigned int n = strlen(str);
    char tmp;

    for (int i = 0; i <= n / 2; ++i)
    {
        tmp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = tmp;
    }
}
