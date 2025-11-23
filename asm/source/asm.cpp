#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include "../headers/asm.h"
#include "../headers/font.h"

int main(int argc, const char* argv[])
{
    size_t file_size = 0;
    FILE* asm_file = fopen(argv[1], "r");
    FILE* bytecode_file = fopen(argv[2], "wb");

    parse_argc_argv(argc, argv);
    check_files(asm_file, bytecode_file);

    char* asm_code_buffer = read_file_to_buffer(asm_file, &file_size);

    asm_code_data asm_code =
    {
        .array_of_pointers = NULL,
        .number_of_strings = 0
    };

    asm_code.array_of_pointers = strings_addresses_to_array(asm_code_buffer, &asm_code.number_of_strings);

    char* bytecode_buffer = (char*) calloc(asm_code.number_of_strings * (sizeof(int) + sizeof(double)) + sizeof(int), sizeof(*bytecode_buffer));

    char* start_of_bytecode_buffer = bytecode_buffer;
    bytecode_buffer += sizeof(int);

    for (size_t i = 0; i < asm_code.number_of_strings; i++)
    {
        string_to_bytecode(asm_code.array_of_pointers[i], &bytecode_buffer);
    }

    int bytecode_char_size = (bytecode_buffer - start_of_bytecode_buffer) - sizeof(int);

    fprintf(bytecode_file, "%d", bytecode_char_size);
    fwrite(start_of_bytecode_buffer + sizeof(int), sizeof(char), bytecode_char_size, bytecode_file);

    fclose(asm_file);
    fclose(bytecode_file);
    free(start_of_bytecode_buffer);

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
    return 0;
}

asm_error_code string_to_bytecode(const char* asm_code_string, char** const bytecode_buffer)
{
    double value = 0;
    commands command_code = NO_COMMAND;

    if (strncmp(asm_code_string, "PUSH", 4) == 0)
    {
        command_code = PUSH;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));

        sscanf(asm_code_string, "%*s %lf", &value);
        memcpy(*bytecode_buffer, &value, sizeof(double));
        *bytecode_buffer += (sizeof(double));
    }

    else if (strcmp(asm_code_string, "POP") == 0)
    {
        command_code = POP;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));
    }

    else if (strcmp(asm_code_string, "ADD") == 0)
    {
        command_code = ADD;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));
    }

    else if (strcmp(asm_code_string, "SUB") == 0)
    {
        command_code = SUB;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));
    }

    else if (strcmp(asm_code_string, "MUL") == 0)
    {
        command_code = MUL;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));
    }

    else if (strcmp(asm_code_string, "DIV") == 0)
    {
        command_code = DIV;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));
    }
    else if (strcmp(asm_code_string, "OUT") == 0)
    {
        command_code = OUT;
        memcpy(*bytecode_buffer, &command_code, sizeof(int));
        *bytecode_buffer += (sizeof(int));
    }

    else
        return READING_ERROR;

    return NO_ERROR;
}

char* read_file_to_buffer(FILE* const asm_file, size_t* const file_size)
{
    assert(asm_file);

    struct stat file_struct = {};
    fstat(fileno(asm_file), &file_struct);
    *file_size = file_struct.st_size;

    char* buffer = (char*) calloc(*file_size + 1, sizeof(*buffer));
    *(buffer + fread(buffer, sizeof(*buffer), *file_size, asm_file)) = '\0';

    return buffer;
}

void parse_argc_argv(int argc, const char* argv[])
{
    if (argc != CORRECT_NUMBER_OF_FILES)
    {
        printf(MAKE_BOLD_RED("Please, use: %s asm_file .\n\n"), argv[0]);
        exit(EXIT_FAILURE);
    }
}

void check_files(const FILE* const asm_file, const FILE* const bytecode_file)
{
    if (!asm_file)
    {
        printf(MAKE_BOLD_RED("Can't open asm file.\n\n"));
        exit(EXIT_FAILURE);
    }

    if (!bytecode_file)
    {
        printf(MAKE_BOLD_RED("Can't open bytecode file.\n\n"));
        exit(EXIT_FAILURE);
    }
}

char** strings_addresses_to_array(char* buffer, size_t* const number_of_strings)
{
    assert(buffer);
    assert(number_of_strings);

    char* start_of_buffer = buffer;

    count_strings(buffer, number_of_strings);

    char** array_of_pointers = (char**) calloc(*number_of_strings + 1, sizeof(char*));
    assert(array_of_pointers);

    post_pointers(array_of_pointers, start_of_buffer, buffer);
    assert(*array_of_pointers);

    return array_of_pointers;
}

void count_strings(const char* buffer, size_t* const number_of_strings)
{
    assert(buffer);
    assert(number_of_strings);

    char* string_pointer = 0;

    while ((string_pointer = strchr(buffer, '\n')) != NULL)
    {
        *number_of_strings += 1;
        buffer = string_pointer + 1;
    }
}

void post_pointers(char** const array_of_pointers, char* const start_of_buffer, char* buffer)
{
    assert(array_of_pointers);
    assert(start_of_buffer);
    assert(buffer);

    char* string_pointer = 0;
    buffer = start_of_buffer;
    array_of_pointers[0] = start_of_buffer;

    for (int i = 1; (string_pointer = strchr(buffer, '\n')) != NULL; i++)
    {
        *string_pointer = '\0';
        array_of_pointers[i] = string_pointer + 1;
        buffer = string_pointer + 1;
    }
}
