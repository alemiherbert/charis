#include <stdlib.h>
#include "chunk.h"
#include "memory.h"
#include "value.h"

void init_chunk(chunk_t *chunk)
{
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;

	chunk->lines_count = 0;
	chunk->lines_capacity = 0;
	chunk->lines = NULL;
	init_value_array(&chunk->constants);
}

void free_chunk(chunk_t *chunk)
{
	free_array(chunk->code);
	free_array(chunk->lines);
	free_value_array(&chunk->constants);
	init_chunk(chunk);
}

void write_chunk(chunk_t *chunk, uint8_t byte, int line)
{
	// Grow the code array if necessary
	if (chunk->capacity < chunk->count + 1)
	{
		int old_capacity = chunk->capacity;
		chunk->capacity = grow_capacity(old_capacity);
		chunk->code = (uint8_t *)grow_array(chunk->code, old_capacity, chunk->capacity, sizeof(uint8_t));
	}

	// Write the bytecode
	chunk->code[chunk->count] = byte;
	chunk->count++;

	// Update line information
	if (chunk->lines_count > 0 && chunk->lines[chunk->lines_count - 2] == line)
	{
		chunk->lines[chunk->lines_count - 1]++;
	}
	else
	{
		// Otherwise add a new entry for the new line
		if (chunk->lines_capacity < chunk->lines_count + 2)
		{
			int old_capacity = chunk->lines_capacity;
			chunk->lines_capacity = grow_capacity(old_capacity);
			chunk->lines = (int *)grow_array(chunk->lines, old_capacity, chunk->lines_capacity, sizeof(int));
		}
		chunk->lines[chunk->lines_count] = line;
		chunk->lines[chunk->lines_count + 1] = 1;
		chunk->lines_count += 2;
	}
}

int add_constant(chunk_t *chunk, value_t value)
{
	write_value_array(&chunk->constants, value);
	return (chunk->constants.count - 1);
}

int get_line(chunk_t *chunk, size_t instruction_idx)
{
	size_t current_idx = 0;
	for (size_t i = 0; i < chunk->lines_count; i += 2)
	{
		int line = chunk->lines[i];
		int count = chunk->lines[i + 1];

		if (instruction_idx < current_idx + count)
			return (line);
		current_idx += count;
	}
	return (-1);
}
