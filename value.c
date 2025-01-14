#include <stdio.h>

#include "memory.h"
#include "value.h"

void init_value_array(value_array_t *array)
{
	array->values = NULL;
	array->capacity = 0;
	array->count = 0;
}

void free_value_array(value_array_t *array)
{
	free_array(array->values);
	init_value_array(array);
}

void write_value_array(value_array_t *array, value_t value)
{
	if (array->capacity < array->count + 1)
	{
		int old_capacity = array->capacity;
		array->capacity = grow_capacity(old_capacity);
		array->values = grow_array(array->values, old_capacity, array->capacity, sizeof(value_t));
	}

	array->values[array->count] = value;
	array->count++;
}

void print_value(value_t value)
{
	printf("%g", value);
}