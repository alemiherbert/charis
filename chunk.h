#pragma once

#include <stdint.h>
#include "common.h"

typedef enum opcode_s {
    OP_RETURN,
} opcode_t;

typedef struct chunk_s {
    int count;
    int capacity;
    uint8_t *code;
} chunk_t;

void init_chunk(chunk_t *chunk);
void free_chunk(chunk_t *chunk);
void write_chunk(chunk_t *chunk, uint8_t byte);