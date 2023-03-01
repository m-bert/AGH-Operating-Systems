#include "lib.h"
#include <stdlib.h>

WordCounter init_counter(int size){
    WordCounter counter;

    counter.elements = calloc(size, sizeof(int *));
    counter.size = 0;
    counter.max_size = size;

    return counter;
}