#pragma once

#include "chunk.h"

void disassemble_chunk(chunk_t *chunk, const char *name);
int disassemble_instruction(chunk_t *chunk, int offset);
static int simple_instruction(const char *name, int offset);
static int constant_instruction(const char *name, chunk_t *chunk, int offset);