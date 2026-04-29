#include <stdlib.h>

#include "SPU.h"
#include "stack.h"
#include "font.h"

void error_message(error_code error);
const char* error_to_string(error_code error);

int main(int argc, const char* argv[])
{
    FILE* bytecode_file = nullptr;
    error_code error = check_input(&bytecode_file, argc, argv);
    if (error) return error;

    SPU_data SPU =
    {
        .bytecode_buffer = nullptr,
        .buffer_capacity = 0,
        .buffer_index = 0,
        .registers = nullptr,
        .memory = nullptr
    };

    SPU.registers = (double*) calloc(NUM_OF_REGISTERS, sizeof(double));
    SPU.memory = (double*) calloc(SIZE_OF_MEMORY, sizeof(double));

    SPU.bytecode_buffer = read_bytecode_file_to_buffer(bytecode_file, &(SPU.buffer_capacity));

    stack_data stack = {};
    stack_init(&stack, 1);

    error = bytecode_to_commands(&SPU, &stack);
    if (error) 
    {
        error_message(error);
        stack_destroy(&stack);
        fclose(bytecode_file);
        free(SPU.bytecode_buffer);
        free(SPU.registers);
        free(SPU.memory);
        return error;
    }

    stack_destroy(&stack);
    fclose(bytecode_file);
    free(SPU.bytecode_buffer);
    free(SPU.registers);
    free(SPU.memory);

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
    return 0;
}

void error_message(error_code error)
{
    printf(MAKE_BOLD_RED("Program exit with fail: %s\n"), error_to_string(error));
}

const char* error_to_string(error_code error)
{
    switch(error)
    {
        case SPU_READING_ERROR:         return "SPU_READING_ERROR";
        case SPU_INPUT_FILE_ERROR:      return "SPU_INPUT_FILE_ERROR";
        case SPU_ARGC_ERROR:            return "SPU_ARGC_ERROR";
        case SPU_COND_JUMP_ERROR:       return "SPU_COND_JUMP_ERROR";
        case STACK_NULLPTR:             return "STACK_NULLPTR";
        case DATA_NULLPTR:              return "DATA_NULLPTR";
        case LEFT_CANARY_DIED:          return "LEFT_CANARY_DIED";
        case RIGHT_CANARY_DIED:         return "RIGHT_CANARY_DIED";
        case STACK_OVERFLOW:            return "STACK_OVERFLOW";
        case STACK_EMPTY:               return "STACK_EMPTY";
        case DIVISION_BY_ZERO:          return "DIVISION_BY_ZERO";

        default:                        return "UNKNOWN_ERROR";
    }
}

