#ifndef LIB_H_
#define LIB_H_

typedef struct WordCounter {
    int ** elements;
    int max_size;
    int size;
} WordCounter;

WordCounter init_counter(int size);
void perform_counting(WordCounter word_counter, char* filename);
int get_element_at(WordCounter word_counter, int index);
void remove_element_at(WordCounter word_counter, int index);
void destroy(WordCounter word_counter);

#endif