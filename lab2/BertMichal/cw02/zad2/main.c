#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "reverse.h"

int main(int argc, char *argv[])
{
    reverse_by_byte(argv[1]);

    return 0;
}