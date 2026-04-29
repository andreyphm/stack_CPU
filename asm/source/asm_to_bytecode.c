#include <assert.h>
#include <string.h>

#include "asm_to_bytecode.h"

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
    CMD_INFO(JUMPB,     "JUMPB",   1),
    CMD_INFO(JUMPAE,    "JUMPAE",  1),
    CMD_INFO(JUMPBE,    "JUMPBE",  1),
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

asm_error_code tag_seek(const char* asm_code_string, char** const bytecode_buffer,
                        const char* const start_of_bytecode_buffer, int* const tags_array)
{
    assert(asm_code_string);
    assert(bytecode_buffer);
    assert(*bytecode_buffer);
    assert(start_of_bytecode_buffer);
    assert(tags_array);

    int tag_index = 0;

    if (*asm_code_string == ':')
    {
        sscanf(asm_code_string, ":%d", &tag_index);
        tags_array[tag_index] = (int)(*bytecode_buffer - start_of_bytecode_buffer - (int)sizeof(int));
    }

    else
    {
        *bytecode_buffer += sizeof(int);

        size_t index = CMD_ARRAY_SIZE;
        size_t length = 0;

        for (size_t i = 0; i < CMD_ARRAY_SIZE; i++)
        {
            if (!strncmp(asm_code_string, cmd_array[i].name, cmd_array[i].strlen) &&
                cmd_array[i].strlen > length)
            {
                index = i;
                length = cmd_array[i].strlen; 
            }
        }

        if (index == CMD_ARRAY_SIZE)
            return ASM_READING_ERROR;

        if (cmd_array[index].code != PUSH)
            *bytecode_buffer += sizeof(int) * (size_t)(cmd_array[index].number_of_args);

        else
            *bytecode_buffer += sizeof(double) * (size_t)(cmd_array[index].number_of_args);
    }

    return ASM_NO_ERROR;
}

asm_error_code string_to_bytecode(const char* asm_code_string, char** const bytecode_buffer, int* const tags_array)
{
    assert(asm_code_string);
    assert(bytecode_buffer);
    assert(tags_array);

    size_t index = CMD_ARRAY_SIZE;
    size_t length = 0;

    for (size_t i = 0; i < CMD_ARRAY_SIZE; i++)
    {
        if (!strncmp(asm_code_string, cmd_array[i].name, cmd_array[i].strlen) &&
            cmd_array[i].strlen > length)
        {
            index = i;
            length = cmd_array[i].strlen; 
        }
    }

    if (index == CMD_ARRAY_SIZE)
        return ASM_READING_ERROR;

    asm_error_code error = command_to_bytecode(asm_code_string, bytecode_buffer, &(cmd_array[index]), tags_array);
    if (error) return error;

    return ASM_NO_ERROR;
}

asm_error_code command_to_bytecode(const char* asm_code_string, char** const bytecode_buffer,
                                   const cmd_info_t* cmd, const int* const tags_array)
{
    memcpy(*bytecode_buffer, &(cmd->code), sizeof(int));
    *bytecode_buffer += sizeof(int);

    if (cmd->number_of_args == 0)
        return ASM_NO_ERROR;

    switch(cmd->code)
    {
        case JUMPB:
        case JUMPA:
        case JUMPBE:
        case JUMPAE:
        case JUMPE:
        case JUMPNE:
        case CALL:
        case JUMP:
        {
            int tag_index = 0;
            sscanf(asm_code_string, "%*s :%d", &tag_index);
            memcpy(*bytecode_buffer, tags_array + tag_index, sizeof(int));
            *bytecode_buffer += sizeof(int);
            return ASM_NO_ERROR;
        }

        case PUSH:
        {
            double value = 0;
            sscanf(asm_code_string, "%*s %lf", &value);
            memcpy(*bytecode_buffer, &value, sizeof(double));
            *bytecode_buffer += (sizeof(double));
            return ASM_NO_ERROR;
        }

        case POPM:
        case PUSHM:
        {
            int memory_index = 0;
            sscanf(asm_code_string + cmd->strlen, " [%d]", &memory_index);
            memcpy(*bytecode_buffer, &memory_index, sizeof(int));
            *bytecode_buffer += sizeof(int);
            return ASM_NO_ERROR;
        }

        case POPR:
        case PUSHR:
        {
            size_t index = REG_ARRAY_SIZE;
            size_t length = 0;

            for (size_t i = 0; i < REG_ARRAY_SIZE; i++)
            {
                if (!strncmp(asm_code_string + cmd->strlen + sizeof(char), registers_array[i].name, registers_array[i].strlen)
                    && registers_array[i].strlen > length)
                {
                    index = i;
                    length = registers_array[i].strlen; 
                }
            }
        
            if (index == REG_ARRAY_SIZE)
                return ASM_READING_ERROR;

            memcpy(*bytecode_buffer, &(registers_array[index].code), sizeof(int));
            *bytecode_buffer += sizeof(int);
            return ASM_NO_ERROR;
        }

        default:
            return CMD_TO_BYTECODE_ERROR;
    }
}
