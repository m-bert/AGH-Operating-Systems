#include "lib.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

WordCounter *init_counter(int size)
{
    WordCounter *counter = malloc(sizeof(WordCounter));

    counter->elements = calloc(size, sizeof(void *));
    counter->size = 0;
    counter->max_size = size;

    return counter;
}

void perform_counting(WordCounter *word_counter, char *filename)
{
    char *temp_filename = "/tmp/count.XXXXXX";
    mkstemp(temp_filename);
    unlink(temp_filename);

    char *arg = "";
    strcpy(arg, "wc ");
    strcat(arg, filename);
    strcat(arg, " > ");
    strcat(arg, temp_filename);

    system(arg);
}

int get_element_at(WordCounter *word_counter, int index)
{
    return 0;
}

void remove_element_at(WordCounter *word_counter, int index)
{
    free(word_counter->elements[index]);
}

void destroy(WordCounter *word_counter)
{
    for (int i = 0; i < word_counter->size; ++i)
    {
        free(word_counter->elements[i]);
    }

    free(word_counter);
}