#ifndef ASM_TO_BYTECODE_H
#define ASM_TO_BYTECODE_H

#include <stdio.h>

#include "errors.h"

#define CMD_INFO(code, name, number_of_args) {code, name, sizeof(name) - 1, number_of_args}
#define REG_INFO(code, name) {code, name, sizeof(name) - 1}

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

enum registers_data
{
    NO_REG,
    RAX,
    RBX,
    RCX,
    RDX
};

struct cmd_info_t
{
    int code;
    const char* name;
    size_t strlen;
    int number_of_args;
};

struct register_info_t
{
    int code;
    const char* name;
    size_t strlen;
};

extern const cmd_info_t cmd_array[];
extern const register_info_t registers_array[];

asm_error_code tag_seek(const char* asm_code_string, char** const bytecode_buffer,
                        const char* const start_of_bytecode_buffer, int* const tags_array);
asm_error_code string_to_bytecode(const char* asm_code_string, char** const bytecode_buffer, int* const tags_array);
asm_error_code command_to_bytecode(const char* asm_code_string, char** const bytecode_buffer,
                                   const cmd_info_t* cmd, const int* const tags_array);

#endif //ASM_TO_BYTECODE_H
