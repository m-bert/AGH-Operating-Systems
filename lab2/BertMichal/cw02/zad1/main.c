#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "replace.h"

int main(int argc, char *argv[])
{
    if (!validate_parameters(argc, argv))
    {
        return -1;
    }

    const char to_replace = *argv[1];
    const char to_replace_with = *argv[2];
    const char *input_path = argv[3];
    const char *output_path = argv[4];

    if (!replace_system_lib(to_replace, to_replace_with, input_path, output_path))
    {
        return -1;
    }

    if (!replace_C_lib(to_replace, to_replace_with, input_path, output_path))
    {
        return -1;
    }

    return 0;
}
