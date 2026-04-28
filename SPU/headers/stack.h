#ifndef STACK_H
#define STACK_H

#include <stdio.h>

const double CANARY = (double)0xDEADBABEDEADBABE;
const double POISON = (double)0xBEADFACEBEADFACE;
const double NUMBER_CLOSE_TO_ZERO  = 10e-12;

struct stack_data
{
    double* data;
    size_t size;
    size_t capacity;
};

enum stack_error_code
{
    NO_ERROR            = 0,
    STACK_NULLPTR       = 1,
    DATA_NULLPTR        = 2,
    LEFT_CANARY_DIED    = 4,
    RIGHT_CANARY_DIED   = 8,
    OVERFLOWW           = 16,
    STACK_EMPTY         = 32,
    DIVISION_BY_ZERO    = 64
};

void stack_init(stack_data* const stack, size_t capacity);
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

double stack_popr(stack_data* const stack, const int register_code, double* const registers, int* pointer_error);
double stack_popm(stack_data* const stack, const int memory_index, double* const memory, int* pointer_error);

int is_close_to_zero (double number_being_checked);

#endif // STACK_H
