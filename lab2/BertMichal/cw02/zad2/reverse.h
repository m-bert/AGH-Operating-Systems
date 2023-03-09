#include <stdbool.h>

#ifndef REVERSE_H_
#define REVERSE_H_

bool reverse_file(const char *input_path, const int block_size);
void reverse_str(char *str);

#endif