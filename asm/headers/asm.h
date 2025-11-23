#ifndef ASM_H
#define ASM_H

#include <stdio.h>

const int CORRECT_NUMBER_OF_FILES = 3;

typedef enum
{
    NO_COMMAND = 0,
    PUSH = 1,
    POP  = 2,
    ADD  = 3,
    SUB  = 4,
    MUL  = 5,
    DIV  = 6,
    OUT  = 7
} commands;

typedef enum
{
    NO_ERROR = 0,
    READING_ERROR = 1
} asm_error_code;

typedef struct
{
    char** array_of_pointers;
    size_t number_of_strings;
} asm_code_data;

void parse_argc_argv(int argc, const char* argv[]);
void check_files(const FILE* const asm_file, const FILE* const bytecode_file);

char* read_file_to_buffer(FILE* const asm_file, size_t* const file_size);
void count_strings(const char* buffer, size_t* const number_of_strings);
void post_pointers(char** const array_of_pointers, char* const start_of_buffer, char* buffer);
char** strings_addresses_to_array(char* buffer, size_t* const number_of_strings);
asm_error_code string_to_bytecode(const char* asm_code_string, char** const bytecode_buffer);

#endif //ASM_H
