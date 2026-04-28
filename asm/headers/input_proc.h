#ifndef INPUT_PROC_H
#define INPUT_PROC_H

#include "asm_to_bytecode.h"

struct asm_code_data
{
    char** array_of_pointers;
    size_t number_of_strings;
};

asm_error_code check_files(FILE** const asm_file, FILE** const bytecode_file, int argc, const char* const argv[]);
void bad_argc_message(const char* const* argv);

char* read_file_to_buffer(FILE* const asm_file, size_t* const file_size);
void count_strings(const char* buffer, size_t* const number_of_strings);
void post_pointers(char** const array_of_pointers, char* const start_of_buffer, char* buffer);
char** strings_addresses_to_array(char* buffer, size_t* const number_of_strings);

#endif // INPUT_PROC_H
