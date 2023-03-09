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

    bool system_replace_status = replace_system_lib(to_replace, to_replace_with, input_path, output_path);

    printf("%d", system_replace_status);

    return 0;
}
