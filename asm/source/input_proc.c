#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include "input_proc.h"
#include "font.h"

asm_error_code check_files(FILE** const asm_file, FILE** const bytecode_file, int argc, const char* const argv[])
{
    if (argc == CORRECT_NUMBER_OF_FILES)
    {
        *asm_file = fopen(argv[1], "r");
        if (!*asm_file)
        {
            printf(MAKE_BOLD_RED("Can't open asm file.\n"));
            return ASM_FILE_ERROR;
        }

        *bytecode_file = fopen(argv[2], "wb");
        if (!*bytecode_file)
        {
            fclose(*asm_file);
            printf(MAKE_BOLD_RED("Can't open bytecode file.\n"));
            return BYTECODE_FILE_ERROR;
        }
    }
    else
    {
        bad_argc_message(argv);
        return ASM_ARGC_ERROR;
    }

    return ASM_NO_ERROR;
}

void bad_argc_message(const char* const* argv)
{
    printf(MAKE_BOLD_RED("You haven't entered asm and bytecode files or you entered them incorrectly.\n"));
    printf(MAKE_BOLD("Please, use: %s asm_file bytecode_file.\n\n"), argv[0]);
}

char* read_file_to_buffer(FILE* const asm_file, size_t* const file_size)
{
    assert(asm_file);
    assert(file_size);

    struct stat file_struct = {};
    fstat(fileno(asm_file), &file_struct);
    *file_size = (size_t)file_struct.st_size;

    char* buffer = (char*) calloc(*file_size + 1, sizeof(*buffer));
    *(buffer + fread(buffer, sizeof(*buffer), *file_size, asm_file)) = '\0';

    return buffer;
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

    const char* string_pointer = 0;

    while ((string_pointer = strchr(buffer, '\n')) != nullptr)
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

    for (int i = 1; (string_pointer = strchr(buffer, '\n')) != nullptr; i++)
    {
        *string_pointer = '\0';
        array_of_pointers[i] = string_pointer + 1;
        buffer = string_pointer + 1;
    }
}
