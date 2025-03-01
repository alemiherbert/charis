#pragma once

#include "common.h"

typedef struct
{
    int capacity;
    int count;
    value_t *values;
} value_array_t;

void init_value_array(value_array_t *array);
void write_value_array(value_array_t *array, value_t value);
void free_value_array(value_array_t *array);
void print_value(value_t value);