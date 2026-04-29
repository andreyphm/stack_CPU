#ifndef STACK_H
#define STACK_H

#include "errors.h"

const double CANARY = (double)0xDEADBABEDEADBABE;
const double POISON = (double)0xBEADFACEBEADFACE;
const double NUMBER_CLOSE_TO_ZERO  = 10e-12;

struct stack_data
{
    double* data;
    size_t size;
    size_t capacity;
};

void stack_init(stack_data* const stack, size_t capacity);
error_code stack_push(stack_data* const stack, double value);
double stack_pop(stack_data* const stack, error_code* error = nullptr);
void stack_dump(const stack_data* const stack);
void stack_destroy(stack_data* stack);
error_code stack_verify(const stack_data* const stack);

error_code stack_add(stack_data* const stack);
error_code stack_sub(stack_data* const stack);
error_code stack_mul(stack_data* const stack);
error_code stack_div(stack_data* const stack);
error_code stack_out(stack_data* const stack);

double stack_popr(stack_data* const stack, const int register_code, double* const registers, error_code* error);
double stack_popm(stack_data* const stack, const int memory_index, double* const memory, error_code* error);

bool is_close_to_zero (double number_being_checked);

#endif // STACK_H
