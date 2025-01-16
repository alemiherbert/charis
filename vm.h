#pragma once

#include "common.h"
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    value_t *stack;
    value_t *stack_top;
    size_t stack_capacity;
    uint8_t *ip;
    chunk_t *chunk;
} vm_t;


typedef enum interpret_result_s
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} interpret_result_t;

typedef void (*instruction_handler_t)(void);

void init_vm(void);
void free_vm(void);


interpret_result_t interpret(const char *source);
static interpret_result_t run(void);
void reset_stack(void);
static void grow_stack(void);
void push(value_t value);
value_t pop(void);