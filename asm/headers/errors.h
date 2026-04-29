#ifndef ERRORS_H
#define ERRORS_H

enum asm_error_code
{
    ASM_NO_ERROR = 0,
    ASM_READING_ERROR,
    ASM_FILE_ERROR,
    BYTECODE_FILE_ERROR,
    ASM_ARGC_ERROR,
    CMD_TO_BYTECODE_ERROR
};

#endif // ERRORS_H
