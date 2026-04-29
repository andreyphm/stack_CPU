#ifndef SPU_H
#define SPU_H

#include <stdio.h>

#include "errors.h"

const int CORRECT_NUMBER_OF_FILES = 2;
const int NUM_OF_REGISTERS = 5;
const int SIZE_OF_MEMORY = 100;

enum commands
{
    HLT         =  0,
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
    NO_COMMAND  = 21
};


enum register_data
{
    NO_REG,
    RAX,
    RBX,
    RCX,
    RDX
};

struct SPU_data
{
    char* bytecode_buffer;
    int buffer_capacity;
    int buffer_index;
    double* registers;
    double* memory;
};

struct stack_data;

error_code check_input(FILE** const input_file, int argc, const char* const argv[]);
void bad_argc_message(const char* const* argv);

char* read_bytecode_file_to_buffer(FILE* const bytecode_file, int* buffer_size);
error_code cond_jump_command(SPU_data* const SPU, stack_data* const stack, const commands command_code);
error_code bytecode_to_commands(SPU_data* const SPU, stack_data* const stack);

#endif // SPU_H
