#ifndef SPU_H
#define SPU_H

#include <stdio.h>

#include "stack.h"

const int CORRECT_NUMBER_OF_FILES = 2;
const int NUM_OF_REGISTERS = 5;
const int SIZE_OF_MEMORY = 100;

#define COND_JUMP_COMMAND(COMPARISON_SIGN);                                                 \
{                                                                                           \
    double stack_last_element = stack_pop(stack, (int*)error);                              \
    double stack_second_last_element = stack_pop(stack, (int*)error);                       \
                                                                                            \
    if (stack_second_last_element COMPARISON_SIGN stack_last_element)                       \
    {                                                                                       \
        memcpy(&number_of_element, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));  \
        SPU->buffer_index = number_of_element;                                              \
    }                                                                                       \
                                                                                            \
    else                                                                                    \
        SPU->buffer_index += (int)sizeof(size_t);                                           \
}

enum commands
{
    NO_COMMAND  =  0,
    PUSH        =  1,
    POP         =  2,
    ADD         =  3,
    SUB         =  4,
    MUL         =  5,
    DIV         =  6,
    OUT         =  7,
    POPR        =  8,
    PUSHR       =  9,
    PUSHM       = 10,
    POPM        = 11,
    JUMP        = 12,
    JUMPB       = 13,
    JUMPA       = 14,
    JUMPBE      = 15,
    JUMPAE      = 16,
    JUMPE       = 17,
    JUMPNE      = 18,
    CALL        = 19,
    RET         = 20,
    HLT         = 21
};


enum register_data
{
    NO_REGISTER = 0,
    RAX         = 1,
    RBX         = 2,
    RCX         = 3,
    RDX         = 4,
    RDI         = 5
};

enum SPU_error_code
{
    SPU_NO_ERROR         = 0,
    SPU_READING_ERROR    = 1,
    SPU_INPUT_FILE_ERROR = 2,
    SPU_ARGC_ERROR       = 3
};

struct SPU_data
{
    char* bytecode_buffer;
    int buffer_capacity;
    int buffer_index;
    int number_of_commands;
    double* registers;
    double* memory;
};

SPU_error_code check_input(FILE** const input_file, int argc, const char* const argv[]);
void bad_argc_message(const char* const* argv);

char* read_bytecode_file_to_buffer(FILE* const bytecode_file, int buffer_size);
SPU_error_code bytecode_to_commands(SPU_data* const SPU, stack_data* const stack);

#endif // SPU_H
