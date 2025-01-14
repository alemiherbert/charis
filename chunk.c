#include <stdlib.h>
#include "chunk.h"
#include "memory.h"

void init_chunk(chunk_t *chunk)
{
    chunk->count = 0;
    chunk->code = 0;
    chunk->code = NULL;
}

void free_chunk(chunk_t *chunk)
{
    free_array(chunk);
    init_chunk(chunk);
}

void write_chunk(chunk_t *chunk, uint8_t byte)
{
    if (chunk->capacity < chunk->count + 1)
    {
        int old_capacity = chunk->capacity;
        chunk->capacity = grow_capacity(old_capacity);
        chunk->code = (uint8_t *)grow_array(chunk->code, old_capacity, chunk->capacity, sizeof(uint8_t));
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}
