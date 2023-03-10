#include <stdbool.h>
#include <stdio.h>

#ifndef REVERSE_H_
#define REVERSE_H_

bool reverse_file(const char *input_path, const int block_size, FILE *input_file, FILE *output_file);
void reverse_str(char *str);
bool perform_test(const char *input_path, const int block_size, FILE *input_file, FILE *output_file);

#endif