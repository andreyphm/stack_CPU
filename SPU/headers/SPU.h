#ifndef SPU_H
#define SPU_H

#include <stdio.h>

#include "../headers/stack.h"

const int CORRECT_NUMBER_OF_FILES = 2;

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
    SPU_NO_ERROR = 0,
    SPU_READING_ERROR = 1
} SPU_error_code;

typedef struct
{
    char* bytecode_buffer;
    int buffer_capacity;
    int buffer_index;
} SPU_data;

char* read_bytecode_file_to_buffer(FILE* const bytecode_file, int* buffer_size);
SPU_error_code bytecode_to_commands(SPU_data* SPU, stack_data* stack);

void parse_argc_argv(int argc, const char* argv[]);
void check_bytecode_file(const FILE* const bytecode_file);

#endif //SPU_H
