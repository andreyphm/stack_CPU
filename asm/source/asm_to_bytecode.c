#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include "asm_to_bytecode.h"
#include "font.h"

asm_error_code tag_seek(const char* asm_code_string, const char* bytecode_buffer,
                        const char* const start_of_bytecode_buffer, size_t* const tags_array)
{
    assert(asm_code_string);
    assert(bytecode_buffer);
    assert(start_of_bytecode_buffer);
    assert(tags_array);

    size_t tag_index = 0;

    if (*asm_code_string == ':')
    {
        sscanf(asm_code_string, ":%lu", &tag_index);
        tags_array[tag_index] = (size_t)(bytecode_buffer - start_of_bytecode_buffer) - sizeof(int);
    }

    else
    {
        bytecode_buffer += sizeof(int);

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

        bytecode_buffer += sizeof(int) * (size_t)(cmd_array[index].number_of_args);
    }

    return ASM_NO_ERROR;
}

asm_error_code string_to_bytecode(const char* asm_code_string, char** const bytecode_buffer, size_t* const tags_array)
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
                                   const cmd_info_t* cmd, const size_t* const tags_array)
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
            size_t tag_index = 0;
            sscanf(asm_code_string, "%*s :%lu", &tag_index);
            memcpy(*bytecode_buffer, tags_array + tag_index, sizeof(size_t));
            *bytecode_buffer += sizeof(size_t);
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
            sscanf(asm_code_string + cmd->strlen, "[%d]", &memory_index);
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
                if (!strncmp(asm_code_string + cmd->strlen, registers_array[i].name, registers_array[i].strlen) &&
                    registers_array[i].strlen > length)
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
