#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include "../headers/SPU.h"
#include "../headers/stack.h"
#include "../headers/font.h"

int main(int argc, const char* argv[])
{
    FILE* bytecode_file = fopen(argv[1], "rb");

    parse_argc_argv(argc, argv);
    check_bytecode_file(bytecode_file);

    SPU_data SPU =
    {
        .bytecode_buffer = NULL,
        .buffer_capacity = 0,
        .buffer_index = 0
    };

    SPU.bytecode_buffer = read_bytecode_file_to_buffer(bytecode_file, &(SPU.buffer_capacity));

    stack_data stack_1 = {};
    stack_init(&stack_1, 1);

    bytecode_to_commands(&SPU, &stack_1);

    stack_destroy(&stack_1);
    fclose(bytecode_file);
    free(SPU.bytecode_buffer);

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
    return 0;
}

char* read_bytecode_file_to_buffer(FILE* const bytecode_file, int* buffer_size)
{
    assert(bytecode_file);

    fscanf(bytecode_file, "%d", buffer_size);
    char* bytecode_buffer = (char*) calloc(*buffer_size, sizeof(char));
    fread(bytecode_buffer, sizeof(char), *buffer_size, bytecode_file);

    return bytecode_buffer;
}

SPU_error_code bytecode_to_commands(SPU_data* const SPU, stack_data* const stack)
{
    SPU_error_code error = SPU_NO_ERROR;
    commands command_code = NO_COMMAND;
    double value = 0;

    while (SPU->buffer_index < SPU->buffer_capacity)
    {
        memcpy(&command_code, SPU->bytecode_buffer + SPU->buffer_index, sizeof(int));
        SPU->buffer_index += sizeof(int);

        if (command_code == PUSH)
        {
            memcpy(&value, SPU->bytecode_buffer + SPU->buffer_index, sizeof(double));
            stack_push(stack, value);

            SPU->buffer_index += sizeof(double);
        }

        else if (command_code == POP)
            stack_pop(stack, (int*)error);

        else if (command_code == ADD)
            stack_add(stack);

        else if (command_code == SUB)
            stack_sub(stack);

        else if (command_code == MUL)
            stack_mul(stack);

        else if (command_code == DIV)
            stack_div(stack);

        else if (command_code == OUT)
            stack_out(stack);

        else
        {
            break;
        }
    }

    return error;
}

void parse_argc_argv(int argc, const char* argv[])
{
    if (argc != CORRECT_NUMBER_OF_FILES)
    {
        printf(MAKE_BOLD_RED("Please, use: %s asm_file .\n\n"), argv[0]);
        exit(EXIT_FAILURE);
    }
}

void check_bytecode_file(const FILE* const bytecode_file)
{
    if (!bytecode_file)
    {
        printf(MAKE_BOLD_RED("Can't open bytecode file.\n\n"));
        exit(EXIT_FAILURE);
    }
}
