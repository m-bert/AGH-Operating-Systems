#include "lib.h"
#include <stdlib.h>
#include <stdio.h>
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
    // Create tmp file
    char *tmp_filename = "/tmp/count.XXXXXX";
    mkstemp(tmp_filename);
    unlink(tmp_filename);

    // Execute wc
    char *arg = "";
    strcpy(arg, "wc ");
    strcat(arg, filename);
    strcat(arg, " > ");
    strcat(arg, tmp_filename);

    system(arg);

    // Read file
    FILE *f = fopen(tmp_filename, "rb");
    long length;
    char *buffer;

    if (f)
    {
        fseek(f, 0L, SEEK_END);
        length = ftell(f);
        fseek(f, 0L, SEEK_SET);
        buffer = malloc(length);

        if (buffer)
        {
            fread(buffer, 1, length, f);
        }

        fclose(f);
    }

    if (buffer)
    {
        word_counter->elements[word_counter->size] = calloc(length, sizeof(char));
        memmove(word_counter->elements[word_counter->size], buffer, length);

        word_counter->size++;

        remove(tmp_filename);
    }
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