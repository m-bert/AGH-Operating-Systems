#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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

    clock_t system_start_time = clock();
    bool replace_system_lib_succeed = replace_system_lib(to_replace, to_replace_with, input_path, output_path);
    clock_t system_end_time = clock();

    if (!replace_system_lib_succeed)
    {
        return -1;
    }
    else
    {
        clock_t system_total_time = system_end_time - system_start_time;
        double system_total_ms = system_total_time / (double)CLOCKS_PER_SEC * 1000;

        printf("Replaced %c with %c with system lib in %.5f ms\n", to_replace, to_replace_with, system_total_ms);
    }

    clock_t C_start_time = clock();
    bool replace_C_lib_succeed = replace_C_lib(to_replace, to_replace_with, input_path, output_path);
    clock_t C_end_time = clock();

    if (!replace_C_lib_succeed)
    {
        return -1;
    }
    else
    {
        clock_t C_total_time = C_end_time - C_start_time;
        double C_total_ms = C_total_time / (double)CLOCKS_PER_SEC * 1000;

        printf("Replaced %c with %c with C lib in %.5f ms\n", to_replace, to_replace_with, C_total_ms);
    }

    return 0;
}
