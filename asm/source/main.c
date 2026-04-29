#include <stdlib.h>

#include "input_proc.h"
#include "asm_to_bytecode.h"
#include "font.h"

void error_message(asm_error_code error);
const char* error_to_string(asm_error_code error);

int main(int argc, const char* argv[])
{
    asm_error_code error = ASM_NO_ERROR;
    FILE* asm_file = nullptr;
    FILE* bytecode_file = nullptr;
    error = check_files(&asm_file, &bytecode_file, argc, argv);
    if (error) return error;

    size_t file_size = 0;
    char* asm_code_buffer = read_file_to_buffer(asm_file, &file_size);

    asm_code_data asm_code =
    {
        .array_of_pointers = nullptr,
        .number_of_strings = 0
    };

    asm_code.array_of_pointers = strings_addresses_to_array(asm_code_buffer, &asm_code.number_of_strings);

    char* bytecode_buffer = (char*) calloc(asm_code.number_of_strings * (sizeof(int) + sizeof(double)) + sizeof(int), 
                            sizeof(*bytecode_buffer));

    char* start_of_bytecode_buffer = bytecode_buffer;
    bytecode_buffer += sizeof(int);

    int tags_array[100] = {};
    for (size_t number_of_string = 1; number_of_string <= asm_code.number_of_strings; number_of_string++)
    {
        error = tag_seek(asm_code.array_of_pointers[number_of_string - 1], &bytecode_buffer, start_of_bytecode_buffer, tags_array);
        if (error)
        {
            error_message(error);
            fclose(asm_file);
            fclose(bytecode_file);
            free(start_of_bytecode_buffer);
            return error;
        }
    }

    bytecode_buffer = start_of_bytecode_buffer + sizeof(int);

    for (size_t number_of_string = 1; number_of_string <= asm_code.number_of_strings; number_of_string++)
    {
        const char* asm_code_string = asm_code.array_of_pointers[number_of_string - 1];

        if (*asm_code_string == ':')
            continue;

        error = string_to_bytecode(asm_code_string, &bytecode_buffer, tags_array);
        if (error)
        {
            error_message(error);
            fclose(asm_file);
            fclose(bytecode_file);
            free(start_of_bytecode_buffer);
            return error;
        }
    }

    int bytecode_char_size = (int)(bytecode_buffer - start_of_bytecode_buffer - (int)sizeof(int));

    fprintf(bytecode_file, "%d", bytecode_char_size);
    fwrite(start_of_bytecode_buffer + sizeof(int), sizeof(char), (size_t)bytecode_char_size, bytecode_file);

    fclose(asm_file);
    fclose(bytecode_file);
    free(start_of_bytecode_buffer);

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
    return 0;
}

void error_message(asm_error_code error)
{
    printf(MAKE_BOLD_RED("Program exit with fail: %s\n"), error_to_string(error));
}

const char* error_to_string(asm_error_code error)
{
    switch(error)
    {
        case ASM_READING_ERROR:         return "ASM_READING_ERROR";
        case ASM_FILE_ERROR:            return "ASM_FILE_ERROR";
        case BYTECODE_FILE_ERROR:       return "BYTECODE_FILE_ERROR";
        case ASM_ARGC_ERROR:            return "ASM_ARGC_ERROR";
        case CMD_TO_BYTECODE_ERROR:     return "CMD_TO_BYTECODE_ERROR";

        default:                        return "UNKNOWN_ERROR";
    }
}
