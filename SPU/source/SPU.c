#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include "SPU.h"
#include "stack.h"
#include "font.h"

int main(int argc, const char* argv[])
{
    FILE* bytecode_file = nullptr;
    check_input(&bytecode_file, argc, argv);

    SPU_data SPU =
    {
        .bytecode_buffer = nullptr,
        .buffer_capacity = 0,
        .buffer_index = 0,
        .number_of_commands = 0,
        .registers = nullptr,
        .memory = nullptr
    };

    SPU.registers = (double*) calloc(NUM_OF_REGISTERS, sizeof(double));
    SPU.memory = (double*) calloc(SIZE_OF_MEMORY, sizeof(double));

    SPU.bytecode_buffer = read_bytecode_file_to_buffer(bytecode_file, SPU.buffer_capacity);

    stack_data stack = {};
    stack_init(&stack, 1);

    bytecode_to_commands(&SPU, &stack);

    stack_destroy(&stack);
    fclose(bytecode_file);
    free(SPU.bytecode_buffer);
    free(SPU.registers);
    free(SPU.memory);

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
    return 0;
}

char* read_bytecode_file_to_buffer(FILE* const bytecode_file, int buffer_size)
{
    assert(bytecode_file);

    fscanf(bytecode_file, "%d", &buffer_size);
    char* bytecode_buffer = (char*) calloc((size_t)buffer_size, sizeof(char));
    assert(bytecode_buffer);

    fread(bytecode_buffer, sizeof(char), (size_t)buffer_size, bytecode_file);

    return bytecode_buffer;
}

SPU_error_code bytecode_to_commands(SPU_data* const SPU, stack_data* const stack)
{
    assert(SPU);
    assert(stack);

    SPU_error_code error = SPU_NO_ERROR;
    commands command_code = NO_COMMAND;
    register_data register_code = NO_REGISTER;
    int memory_index = -1;
    double value = 0;
    int number_of_element = 0;

    stack_data return_address_stack = {};
    stack_init(&return_address_stack, 1);

    while (SPU->buffer_index < SPU->buffer_capacity)
    {
        memcpy(&command_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
        SPU->buffer_index += (int)sizeof(int);

        switch(command_code)
        {
            case PUSH:
                memcpy(&value, SPU->bytecode_buffer + SPU->buffer_index, sizeof(double));
                stack_push(stack, value);
                SPU->buffer_index += (int)sizeof(double);
                break;
            
            case POP:
                stack_pop(stack, (int*)error);
                break;

            case ADD:
                stack_add(stack);
                break;

            case SUB:
                stack_sub(stack);
                break;

            case MUL:
                stack_mul(stack);
                break;

            case DIV:
                stack_div(stack);
                break;

            case OUT:
                stack_out(stack);
                break;

            case HLT:
                return error;

            case POPR:
                memcpy(&register_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_popr(stack, register_code, SPU->registers, (int*)error);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case PUSHR:
                memcpy(&register_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_push(stack, SPU->registers[register_code]);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case PUSHM:
                memcpy(&memory_index, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_push(stack, SPU->memory[memory_index]);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case POPM:
                memcpy(&memory_index, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_popm(stack, memory_index, SPU->memory, (int*) error);
                SPU->buffer_index += (int)sizeof(int);
                break;

            case JUMP:
                memcpy(&number_of_element, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                SPU->buffer_index = number_of_element;
                break;

            case JUMPB:
                COND_JUMP_COMMAND(<);
                break;

            case JUMPA:
                COND_JUMP_COMMAND(>);
                break;

            case JUMPBE:
                COND_JUMP_COMMAND(<=);
                break;

            case JUMPAE:
                COND_JUMP_COMMAND(>=);
                break;

            case JUMPE:
                COND_JUMP_COMMAND(==);
                break;

            case JUMPNE:
                COND_JUMP_COMMAND(!=);
                break;

            case CALL:
                memcpy(&number_of_element, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
                stack_push(&return_address_stack, (double)(SPU->buffer_index + (int)sizeof(size_t)));
                SPU->buffer_index = number_of_element;
                break;

            case RET:
            {
                double pop_element = stack_pop(&return_address_stack, (int*) error);
                if (pop_element == POISON) return error;
                SPU->buffer_index = (int) pop_element;
            }

            default:
                break;
        }   
    }

    stack_destroy(&return_address_stack);
    return error;
}
