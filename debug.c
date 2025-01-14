#include <stdio.h>
#include "debug.h"
#include "chunk.h"

void disassemble_chunk(chunk_t *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

int disassemble_instruction(chunk_t *chunk, int offset)
{
    printf("%04d ", offset);
    int current_line = get_line(chunk, offset);

    if (offset > 0)
    {
        int previous_line = get_line(chunk, offset - 1);
        if (current_line == previous_line)
            printf("   | ");
        else
            printf("%4d ", current_line);
    }
    else
    {
        printf("%4d ", current_line);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction)
    {
    case OP_RETURN:
        return simple_instruction("OP_RETURN", offset);
    case OP_CONSTANT:
        return constant_instruction("OP_CONSTANT", chunk, offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return (offset + 1);
    }
}

static int simple_instruction(const char *name, int offset)
{
    printf("%s\n", name);
    return (offset + 1);
}

static int constant_instruction(const char *name, chunk_t *chunk, int offset)
{
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    print_value(chunk->constants.values[constant]);
    printf("'\n");
    return (offset + 2);
}