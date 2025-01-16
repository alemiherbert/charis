#pragma once

#include <stdint.h>
#include "common.h"
#include "value.h"

typedef enum opcode_s
{
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_RETURN,
} opcode_t;

typedef struct chunk_s
{
	int count;
	int capacity;
	uint8_t *code;

	int *lines;
	size_t lines_count;
	size_t lines_capacity;
	value_array_t constants;
} chunk_t;

void init_chunk(chunk_t *chunk);
void free_chunk(chunk_t *chunk);
void write_chunk(chunk_t *chunk, uint8_t byte, int line);
int add_constant(chunk_t *chunk, value_t value);
int get_line(chunk_t *chunk, size_t instruction_idx);