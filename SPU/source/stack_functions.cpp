#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../headers/stack.h"
#include "../headers/font.h"

int stack_init(stack_data* const stack, size_t capacity)
{
    int error = NO_ERROR;

    CHECK_ERROR(!stack, STACK_NULLPTR, error);

    stack->capacity = capacity;
    stack->data = (double*) calloc(stack->capacity + 2, sizeof(double));

    CHECK_ERROR(!(stack->data), DATA_NULLPTR, error);

    stack->data[0] = CANARY;
    stack->data[stack->capacity + 1] = CANARY;

    for (size_t cell_number = 1; cell_number < capacity + 1; cell_number++)
        stack->data[cell_number] = POISON;

    stack->size = 1;

    return error;
}

void stack_destroy(stack_data* stack)
{
    free(stack->data);

    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

int stack_verify(const stack_data* const stack)
{
    int error = NO_ERROR;

    if (!stack)
        error |= STACK_NULLPTR;

    if (!(stack->data))
        error |= DATA_NULLPTR;

    if (stack->data[0] != CANARY)
        error |= LEFT_CANARY_DIED;

    if (stack->data[stack->capacity + 1] != CANARY)
        error |= RIGHT_CANARY_DIED;

    if (stack->size > stack->capacity)
        error |= OVERFLOWW;

    return error;
}

void stack_dump(const stack_data* const stack)
{
    int error = stack_verify(stack);

    if (error & STACK_NULLPTR)
        fprintf(stderr, MAKE_BOLD_RED("STACK_NULLPTR\n"));

    if (error & DATA_NULLPTR)
        fprintf(stderr, MAKE_BOLD_RED("DATA_NULLPTR\n"));

    if (error & LEFT_CANARY_DIED)
        fprintf(stderr, MAKE_BOLD_RED("LEFT_CANARY_DIED\n"));

    if (error & RIGHT_CANARY_DIED)
        fprintf(stderr, MAKE_BOLD_RED("RIGHT_CANARY_DIED\n"));

    if (error & OVERFLOWW)
        fprintf(stderr, MAKE_BOLD_RED("OVERFLOW\n"));

    if (error & STACK_EMPTY)
        fprintf(stderr, MAKE_BOLD_RED("STACK_EMPTY\n"));

    fprintf(stderr, MAKE_BOLD("stack_dump called from ? line ?:\nstack[%p]:\n{\n    size = %Iu\n    capacity = %Iu\n    data[%p]:\n    {\n"),
                                                                                    stack, stack->size, stack->capacity, stack->data);
    for (size_t i = 1; i < stack->size; i++)
    {
        fprintf(stderr, MAKE_BOLD("        [%d] = %lf\n"), i, stack->data[i]);
    }

    fprintf(stderr, MAKE_BOLD("    }\n}\n"));
}

int stack_push(stack_data* const stack, double value)
{
    int error = NO_ERROR;

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    if (stack->size == stack->capacity)
    {
        stack->capacity *= 2;
        stack->data = (double*) realloc(stack->data, (stack->capacity + 2) * sizeof(double));

        CHECK_ERROR(!(stack->data), DATA_NULLPTR, error);

        for (size_t cell_number = stack->capacity / 2 + 1; cell_number < stack->capacity + 1; cell_number++)
            stack->data[cell_number] = POISON;

        stack->data[stack->capacity + 1] = CANARY;
    }

    stack->data[stack->size] = value;
    ++(stack->size);

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    return error;
}

double stack_pop(stack_data* const stack, int* pointer_error)
{
    CHECK_ERROR_FOR_POP(stack);

    if (stack->size == 1)
    {
        if (pointer_error)
            *pointer_error |= STACK_EMPTY;

        fprintf(stderr, MAKE_BOLD_RED("Error code: %d\n"), STACK_EMPTY);
        return POISON;
    }

    CHECK_ERROR_FOR_POP(stack);

    return stack->data[--(stack->size)];
}

int stack_add(stack_data* const stack)
{
    int error = NO_ERROR;

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    double last_value = stack_pop(stack);
    double second_last_value = stack_pop(stack);

    stack_push(stack, second_last_value + last_value);

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    return error;
}

int stack_sub(stack_data* const stack)
{
    int error = NO_ERROR;

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    double last_value = stack_pop(stack);
    double second_last_value = stack_pop(stack);

    stack_push(stack, second_last_value - last_value);

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    return error;
}

int stack_mul(stack_data* const stack)
{
    int error = NO_ERROR;

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    double last_value = stack_pop(stack);
    double second_last_value = stack_pop(stack);

    stack_push(stack, second_last_value * last_value);

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    return error;
}

int stack_div(stack_data* const stack)
{
    int error = NO_ERROR;

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    double last_value = stack_pop(stack);
    double second_last_value = stack_pop(stack);

    if (is_close_to_zero(last_value))
    {
        error |= DIVISION_BY_ZERO;

        fprintf(stderr, MAKE_BOLD_RED("Error code: %d\n"), error);
        return error;
    }

    else
        stack_push(stack, second_last_value / last_value);

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    return error;
}

int stack_out(stack_data* const stack)
{
    int error = NO_ERROR;

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    if (stack->size == 1)
    {
        error |= STACK_EMPTY;

        fprintf(stderr, MAKE_BOLD_RED("Error code: %d\n"), STACK_EMPTY);
        return error;
    }

    printf(MAKE_BOLD("stack->data[%d] = %lf\n"), stack->size - 1, stack->data[stack->size - 1]);
    --(stack->size);

    error = stack_verify(stack);
    CHECK_ERROR(error, 0, error);

    return error;
}

int is_close_to_zero (double number_being_checked)
{
    return (fabs(number_being_checked) < NUMBER_CLOSE_TO_ZERO);
}
