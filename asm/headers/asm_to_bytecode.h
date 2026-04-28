#ifndef ASM_TO_BYTECODE_H
#define ASM_TO_BYTECODE_H

#include <stdio.h>

const int CORRECT_NUMBER_OF_FILES = 3;

#define CMD_INFO(code, name, number_of_args) {code, name, sizeof(name) - 1, number_of_args}
#define REG_INFO(code, name) {code, name, sizeof(name) - 1}

enum commands
{
    HLT,
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    OUT,
    POPR,
    PUSHR,
    PUSHM,
    POPM,
    JUMP,
    JUMPB,
    JUMPA,
    JUMPBE,
    JUMPAE,
    JUMPE,
    JUMPNE,
    CALL,
    RET
};

enum asm_error_code
{
    ASM_NO_ERROR            = 0,
    ASM_READING_ERROR       = 1,
    ASM_FILE_ERROR          = 2,
    BYTECODE_FILE_ERROR     = 3,
    ASM_ARGC_ERROR          = 4,
    CMD_TO_BYTECODE_ERROR   = 5
};

enum registers_data
{
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

const cmd_info_t cmd_array[] =
{
    CMD_INFO(ADD,       "ADD",     0),
    CMD_INFO(SUB,       "SUB",     0),
    CMD_INFO(MUL,       "MUL",     0),
    CMD_INFO(DIV,       "DIV",     0),
    CMD_INFO(POP,       "POP",     0),
    CMD_INFO(OUT,       "OUT",     0),
    CMD_INFO(RET,       "RET",     0),
    CMD_INFO(HLT,       "HLT",     0),
    CMD_INFO(PUSH,      "PUSH",    1),
    CMD_INFO(POPR,      "POPR",    1),
    CMD_INFO(PUSHR,     "PUSHR",   1),
    CMD_INFO(PUSHM,     "PUSHM",   1),
    CMD_INFO(POPM,      "POPM",    1),
    CMD_INFO(JUMP,      "JUMP",    1),
    CMD_INFO(JUMPA,     "JUMPA",   1),
    CMD_INFO(JUMPBE,    "JUMPBE",  1),
    CMD_INFO(JUMPAE,    "JUMPAE",  1),
    CMD_INFO(JUMPE,     "JUMPE",   1),
    CMD_INFO(JUMPNE,    "JUMPNE",  1),
    CMD_INFO(CALL,      "CALL",    1)
};

const size_t CMD_ARRAY_SIZE = sizeof(cmd_array)/sizeof(cmd_array[0]);

const register_info_t registers_array[] =
{
    REG_INFO(RAX, "RAX"),
    REG_INFO(RBX, "RBX"),
    REG_INFO(RCX, "RCX"),
    REG_INFO(RDX, "RDX")
};

const size_t REG_ARRAY_SIZE = sizeof(registers_array)/sizeof(registers_array[0]);

asm_error_code tag_seek(const char* asm_code_string, const char* bytecode_buffer,
                        const char* const start_of_bytecode_buffer, size_t* const tags_array);
asm_error_code string_to_bytecode(const char* asm_code_string, char** const bytecode_buffer, size_t* const tags_array);
asm_error_code command_to_bytecode(const char* asm_code_string, char** const bytecode_buffer,
                                   const cmd_info_t* cmd, const size_t* const tags_array);

#endif //ASM_TO_BYTECODE_H
