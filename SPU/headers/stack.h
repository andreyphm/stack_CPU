#ifndef STACK_H
#define STACK_H

#include <stdio.h>

const double CANARY = (double)0xDEADBABEDEADBABE;
const double POISON = (double)0xBEADFACEBEADFACE;
const double NUMBER_CLOSE_TO_ZERO  = 10e-12;

typedef struct
{
    double* data;
    size_t size;
    size_t capacity;
} stack_data;

typedef enum
{
    NO_ERROR = 0,
    STACK_NULLPTR = 1,
    DATA_NULLPTR = 2,
    LEFT_CANARY_DIED = 4,
    RIGHT_CANARY_DIED = 8,
    OVERFLOWW = 16,
    STACK_EMPTY = 32,
    DIVISION_BY_ZERO = 64
} stack_error_code;

#define CHECK_ERROR(VALUE, ERROR_CODE, ERROR) ({                   \
    if (VALUE)                                                     \
    {                                                              \
        ERROR |= ERROR_CODE;                                       \
        fprintf(stderr, MAKE_BOLD_RED("Error code: %d\n"), ERROR); \
        return ERROR;                                              \
    } })

#define CHECK_ERROR_FOR_POP(stack) ({                                           \
    if (pointer_error)                                                          \
    {                                                                           \
        *pointer_error = stack_verify(stack);                                   \
        if (*pointer_error)                                                     \
        {                                                                       \
            fprintf(stderr, MAKE_BOLD_RED("Error code: %d\n"), *pointer_error); \
            return POISON;                                                      \
        }                                                                       \
    } })

int stack_init(stack_data* const stack, size_t first_size_of_stack);
int stack_push(stack_data* const stack, double value);
double stack_pop(stack_data* const stack, int* pointer_error = NULL);
void stack_dump(const stack_data* const stack);
void stack_destroy(stack_data* stack);
int stack_verify(const stack_data* const stack);

int stack_add(stack_data* const stack);
int stack_sub(stack_data* const stack);
int stack_mul(stack_data* const stack);
int stack_div(stack_data* const stack);
int stack_out(stack_data* const stack);

void string_to_command(const char* asm_code_string, stack_data* stack);
int is_close_to_zero (double number_being_checked);

#endif // STACK_H
