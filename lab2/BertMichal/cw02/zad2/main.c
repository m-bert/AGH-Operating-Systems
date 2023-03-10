#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "reverse.h"

int main(int argc, char *argv[])
{
    reverse_file(argv[1], 1);

    return 0;
}