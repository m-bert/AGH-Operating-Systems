#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "reverse.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments (maybe you forgot input path)\n");
        return -1;
    }

    char *input_path = argv[1];

    perform_test(input_path, 1);
    perform_test(input_path, 1024);

    return 0;
}