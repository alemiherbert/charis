#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "vm.h"

value_t bool_val(bool value)
{
	value_t result;
	result.type = VAL_BOOLEAN;
	result.as.boolean = value;
	return (result);
}

value_t null_val(void)
{
	value_t result;
	result.type = VAL_NULL;
	result.as.number = 0;
	return (result);
}

value_t number_val(double value)
{
	value_t result;
	result.type = VAL_NUMBER;
	result.as.number = value;
	return (result);
}

bool as_bool(value_t value)
{
	if (!is_bool(value))
	{
		fprintf(stderr, "Error: Expected boolean value.\n");
		exit(1);
	}
	return (value.as.boolean);
}

double as_number(value_t value)
{
	if (!is_number(value))
	{
		fprintf(stderr, "Error: Expected number value.\n");
		exit(1);
	}
	return value.as.number;
}

bool is_bool(value_t value) { return (value.type == VAL_BOOLEAN); }

bool is_null(value_t value) { return (value.type == VAL_NULL); }

bool is_number(value_t value) { return (value.type == VAL_NUMBER); }
