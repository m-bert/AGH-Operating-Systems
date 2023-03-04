#include <stdio.h>
#include "lib.h"

int main(){
    WordCounter *word_counter = init_counter(10);
    perform_counting(word_counter, "lib.h");

    return 0;
}