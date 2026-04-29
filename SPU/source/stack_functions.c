#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "stack.h"
#include "font.h"

void stack_init(stack_data* const stack, size_t capacity)
{
    assert(stack);

    stack->capacity = capacity;
    stack->data = (double*) calloc(stack->capacity + 2, sizeof(double));

    stack->data[0] = CANARY;
    stack->data[stack->capacity + 1] = CANARY;

    for (size_t cell_number = 1; cell_number < capacity + 1; cell_number++)
        stack->data[cell_number] = POISON;

    stack->size = 1;
}

void stack_destroy(stack_data* stack)
{
    assert(stack);

    free(stack->data);

    stack->data = nullptr;
    stack->size = 0;
    stack->capacity = 0;
}

error_code stack_verify(const stack_data* const stack)
{
    error_code error = NO_ERROR;

    if (!stack) return STACK_NULLPTR;
    if (!(stack->data)) return DATA_NULLPTR;
    if (!is_close_to_zero(stack->data[0] - CANARY)) return LEFT_CANARY_DIED;
    if (!is_close_to_zero(stack->data[stack->capacity + 1] - CANARY)) return RIGHT_CANARY_DIED;
    if (stack->size > stack->capacity + 1) return STACK_OVERFLOW;

    return error;
}

void stack_dump(const stack_data* const stack)
{
    error_code error = stack_verify(stack);

    switch(error)
    {
        case STACK_NULLPTR:
            fprintf(stderr, MAKE_BOLD_RED("STACK_NULLPTR\n"));
            return;

        case DATA_NULLPTR:
            fprintf(stderr, MAKE_BOLD_RED("DATA_NULLPTR\n"));
            return;

        case LEFT_CANARY_DIED:
            fprintf(stderr, MAKE_BOLD_RED("LEFT_CANARY_DIED\n"));
            break;

        case RIGHT_CANARY_DIED:
            fprintf(stderr, MAKE_BOLD_RED("RIGHT_CANARY_DIED\n"));
            break;

        case STACK_OVERFLOW:
            fprintf(stderr, MAKE_BOLD_RED("STACK_OVERFLOW\n"));
            break;

        case STACK_EMPTY:
            fprintf(stderr, MAKE_BOLD_RED("STACK_EMPTY\n"));
            break;

        case DIVISION_BY_ZERO:
            fprintf(stderr, MAKE_BOLD_RED("DIVISION_BY_ZERO\n"));
            break;

        case NO_ERROR:
            fprintf(stderr, MAKE_BOLD_GREEN("NO_ERROR\n"));
            break;

        default:
            fprintf(stderr, MAKE_BOLD_RED("UNKNOWN_ERROR\n"));
            break;
    }

    fprintf(stderr, MAKE_BOLD("stack_dump called from ? line ?:\nstack[%p]:\n{\n    "
                              "size = %zu\n    capacity = %zu\n    data[%p]:\n    {\n"),
                               stack, stack->size, stack->capacity, stack->data);

    for (size_t i = 1; i < stack->size; i++)
        fprintf(stderr, MAKE_BOLD("        [%zu] = %lf\n"), i, stack->data[i]);

    fprintf(stderr, MAKE_BOLD("    }\n}\n"));
}

error_code stack_push(stack_data* const stack, double value)
{
    error_code error = NO_ERROR;

    error = stack_verify(stack);
    if (error) return error;

    if (stack->size == stack->capacity + 1)
    {
        stack->capacity *= 2;
        stack->data = (double*) realloc(stack->data, (stack->capacity + 2) * sizeof(double));

        for (size_t cell_number = stack->capacity / 2 + 1; cell_number <= stack->capacity; cell_number++)
            stack->data[cell_number] = POISON;

        stack->data[stack->capacity + 1] = CANARY;

        error = stack_verify(stack);
        if (error) return error;
    }

    stack->data[stack->size] = value;
    ++(stack->size);

    error = stack_verify(stack);
    if (error) return error;

    return error;
}

double stack_pop(stack_data* const stack, error_code* error_ptr)
{
    error_code error = stack_verify(stack);
    if (error)
    {
        if (error_ptr)
            *error_ptr = error;
        return POISON;
    }

    if (stack->size == 1)
    {
        if (error_ptr)
            *error_ptr = STACK_EMPTY;

        return POISON;
    }

    error = stack_verify(stack);
    if (error)
    {
        if (error_ptr)
            *error_ptr = error;
        return POISON;
    }

    double pop_element = stack->data[stack->size - 1];
    stack->data[--stack->size] = POISON;

    return pop_element;
}

error_code stack_add(stack_data* const stack)
{
    error_code error = NO_ERROR;

    error = stack_verify(stack);
    if (error) return error;

    double last_value = stack_pop(stack, &error);
    if (error) return error;

    double second_last_value = stack_pop(stack, &error);
    if (error) return error;

    error = stack_push(stack, second_last_value + last_value);
    if (error) return error;

    error = stack_verify(stack);
    if (error) return error;

    return error;
}

error_code stack_sub(stack_data* const stack)
{
    error_code error = NO_ERROR;

    error = stack_verify(stack);
    if (error) return error;

    double last_value = stack_pop(stack, &error);
    if (error) return error;

    double second_last_value = stack_pop(stack, &error);
    if (error) return error;

    error = stack_push(stack, second_last_value - last_value);
    if (error) return error;

    error = stack_verify(stack);
    if (error) return error;

    return error;
}

error_code stack_mul(stack_data* const stack)
{
    error_code error = NO_ERROR;

    error = stack_verify(stack);
    if (error) return error;

    double last_value = stack_pop(stack, &error);
    if (error) return error;

    double second_last_value = stack_pop(stack, &error);
    if (error) return error;

    error = stack_push(stack, second_last_value * last_value);
    if (error) return error;

    error = stack_verify(stack);
    if (error) return error;

    return error;
}

error_code stack_div(stack_data* const stack)
{
    error_code error = NO_ERROR;

    error = stack_verify(stack);
    if (error) return error;

    double last_value = stack_pop(stack, &error);
    if (error) return error;

    double second_last_value = stack_pop(stack, &error);
    if (error) return error;

    if (is_close_to_zero(last_value))
        return DIVISION_BY_ZERO;

    else
    {
        error = stack_push(stack, second_last_value / last_value);
        if (error) return error;
    }

    error = stack_verify(stack);
    if (error) return error;

    return error;
}

error_code stack_out(stack_data* const stack)
{
    error_code error = NO_ERROR;

    error = stack_verify(stack);
    if (error) return error;

    if (stack->size == 1)
        return STACK_EMPTY;

    printf(MAKE_BOLD("stack->data[%zu] = %lf\n"), stack->size - 1, stack->data[stack->size - 1]);
    --(stack->size);

    error = stack_verify(stack);
    if (error) return error;

    return error;
}

double stack_popr(stack_data* const stack, const int register_code, double* const registers, error_code* error)
{
    assert(registers);
    assert(error);

    *error = stack_verify(stack);
    if (*error) return POISON;

    if (stack->size == 1)
    {
        if (error)
            *error = STACK_EMPTY;

        return POISON;
    }

    *error = stack_verify(stack);
    if (*error) return POISON;

    double pop_element = stack->data[stack->size - 1];
    stack->data[--stack->size] = POISON;

    registers[register_code] = pop_element;

    return pop_element;
}

double stack_popm(stack_data* const stack, const int memory_index, double* const memory, error_code* error)
{
    assert(memory);
    assert(error);

    *error = stack_verify(stack);
    if (*error) return POISON;

    if (stack->size == 1)
    {
        if (error)
            *error = STACK_EMPTY;

        return POISON;
    }

    *error = stack_verify(stack);
    if (*error) return POISON;

    double pop_element = stack->data[stack->size - 1];
    stack->data[--stack->size] = POISON;

    memory[memory_index] = pop_element;

    return pop_element;
}

bool is_close_to_zero (double number_being_checked)
{
    return (fabs(number_being_checked) < NUMBER_CLOSE_TO_ZERO);
}
