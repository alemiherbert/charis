#pragma once
#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

// #define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

typedef enum value_type_s
{
	VAL_BOOLEAN,
	VAL_NULL,
	VAL_NUMBER,
} value_type_t;

typedef struct value_s
{
	value_type_t type;
	union
	{
		bool boolean;
		double number;
	} as;
} value_t;


value_t bool_val(bool value);
value_t null_val(void);
value_t number_val(double value);

bool as_bool(value_t value);
double as_number(value_t value);

bool is_bool(value_t value);
bool is_null(value_t value);
bool is_number(value_t value);
#endif // COMMON_H