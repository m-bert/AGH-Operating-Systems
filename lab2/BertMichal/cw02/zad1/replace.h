#include <stdbool.h>

#ifndef REPLACE_H_
#define REPLACE_H_

bool validate_parameters(int argc, char *argv[]);
bool replace_system_lib(const char to_replace, const char to_replace_with, const char *input_path, const char *output_path);
bool replace_C_lib(const char to_replace, const char to_replace_with, const char *input_path, const char *output_path);

#endif