#include "SPU.h"
#include "font.h"

error_code check_input(FILE** const input_file, int argc, const char* const argv[])
{
    if (argc == CORRECT_NUMBER_OF_FILES)
    {
        *input_file = fopen(argv[1], "rb");

        if (!*input_file)
        {
            printf(MAKE_BOLD_RED("Can't open input file.\n"));
            return SPU_INPUT_FILE_ERROR;
        }
    }

    else
    {
        bad_argc_message(argv);
        return SPU_ARGC_ERROR;
    }

    return NO_ERROR;
}

void bad_argc_message(const char* const* argv)
{
    printf(MAKE_BOLD_RED("You haven't entered the input file or you entered it incorrectly.\n"));
    printf(MAKE_BOLD("Please, use: %s input_file.\n\n"), argv[0]);
}
