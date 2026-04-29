#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "SPU.h"
#include "stack.h"

char* read_bytecode_file_to_buffer(FILE* const bytecode_file, int* buffer_size)
{
    assert(bytecode_file);

    fscanf(bytecode_file, "%d", buffer_size);
    char* bytecode_buffer = (char*) calloc((size_t)(*buffer_size), sizeof(char));

    fread(bytecode_buffer, sizeof(char), (size_t)(*buffer_size), bytecode_file);

    return bytecode_buffer;
}

error_code cond_jump_command(SPU_data* const SPU, stack_data* const stack, const commands command_code)
{
    assert(SPU);
    assert(stack);

    error_code error = NO_ERROR;

    double stack_last_element = stack_pop(stack, &error);
    if (error) return error;

    double stack_second_last_element = stack_pop(stack, &error);
    if (error) return error;

    int number_of_element = 0;
    memcpy(&number_of_element, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));

    bool is_jump = false;
    switch(command_code)
    {
        case JUMPA:
            is_jump = (stack_second_last_element > stack_last_element);
            break;

        case JUMPB:
            is_jump = (stack_second_last_element < stack_last_element);
            break;

        case JUMPAE:
            is_jump = (stack_second_last_element > stack_last_element || 
                       is_close_to_zero(stack_second_last_element - stack_last_element));
            break;
        
        case JUMPBE:
            is_jump = (stack_second_last_element < stack_last_element || 
                       is_close_to_zero(stack_second_last_element - stack_last_element));
            break;
            
        case JUMPE:
            is_jump = is_close_to_zero(stack_second_last_element - stack_last_element);
            break;

        case JUMPNE:
            is_jump = !is_close_to_zero(stack_second_last_element - stack_last_element);
            break;

            default:
                return SPU_COND_JUMP_ERROR;
    }

    if (is_jump)
        SPU->buffer_index = number_of_element;
    else
        SPU->buffer_index += (int)sizeof(int);

    return NO_ERROR;
}

error_code bytecode_to_commands(SPU_data* const SPU, stack_data* const stack)
{
    assert(SPU);
    assert(stack);

    error_code error = NO_ERROR;
    commands command_code = NO_COMMAND;
    register_data register_code = NO_REG;
    int memory_index = -1;
    double value = 0;
    int number_of_element = 0;

    stack_data return_address_stack = {};
    stack_init(&return_address_stack, 1);

    while (SPU->buffer_index < SPU->buffer_capacity && !error)
    {
        memcpy(&command_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
        SPU->buffer_index += (int)sizeof(int);

        switch(command_code)
        {
            case PUSH:
                memcpy(&value, SPU->bytecode_buffer + SPU->buffer_index, sizeof(double));
                error = stack_push(stack, value);
                SPU->buffer_index += (int)sizeof(double);
                break;

            case POP:
                stack_pop(stack, &error);
                break;

            case ADD:
                error = stack_add(stack);
                break;

            case SUB:
                error = stack_sub(stack);
                break;

            case MUL:
                error = stack_mul(stack);
                break;

            case DIV:
                error = stack_div(stack);
                break;

            case OUT:
                error = stack_out(stack);
                break;

            case HLT:
                break;

            case POPR:
                memcpy(&register_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_popr(stack, register_code, SPU->registers, &error);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case PUSHR:
                memcpy(&register_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                error = stack_push(stack, SPU->registers[register_code]);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case PUSHM:
                memcpy(&memory_index, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                error = stack_push(stack, SPU->memory[memory_index]);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case POPM:
                memcpy(&memory_index, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_popm(stack, memory_index, SPU->memory, &error);
                
                SPU->buffer_index += (int)sizeof(int);
                break;

            case JUMP:
                memcpy(&number_of_element, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                SPU->buffer_index = number_of_element;
                break;

            case JUMPA:
            case JUMPB:
            case JUMPAE:
            case JUMPBE:
            case JUMPE:
            case JUMPNE:
                error = cond_jump_command(SPU, stack, command_code);
                break;

            case CALL:
                memcpy(&number_of_element, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                error = stack_push(&return_address_stack, (double)(SPU->buffer_index + (int)sizeof(int)));
                SPU->buffer_index = number_of_element;
                break;

            case RET:
            {
                double pop_element = stack_pop(&return_address_stack, &error);
                SPU->buffer_index = (int)pop_element;
            }

            default:
                break;
        }

        if (command_code == HLT) break;
    }

    stack_destroy(&return_address_stack);
    return error;
}

