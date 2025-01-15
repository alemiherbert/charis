#include <stdio.h>
#include "debug.h"
#include "chunk.h"

/**
 * disassemble_chunk - Disassembles a chunk of bytecode.
 * @chunk: Pointer to the chunk to disassemble.
 * @name: Name of the chunk (for display purposes).
 *
 * This function prints the bytecode instructions in the chunk,
 * along with their offsets and line numbers.
 */
void disassemble_chunk(chunk_t *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

/**
 * disassemble_instruction - Disassembles a single instruction in the chunk.
 * @chunk: Pointer to the chunk containing the instruction.
 * @offset: Offset of the instruction in the chunk's code array.
 *
 * This function prints the instruction's offset, line number, and opcode.
 * It returns the offset of the next instruction.
 *
 * Return: The offset of the next instruction.
 */
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

/**
 * simple_instruction - Prints a simple instruction (no operands).
 * @name: Name of the instruction.
 * @offset: Offset of the instruction in the chunk's code array.
 *
 * This function prints the instruction's name and returns the offset
 * of the next instruction.
 *
 * Return: The offset of the next instruction.
 */
static int simple_instruction(const char *name, int offset)
{
    printf("%s\n", name);
    return (offset + 1);
}

/**
 * constant_instruction - Prints a constant instruction (with operands).
 * @name: Name of the instruction.
 * @chunk: Pointer to the chunk containing the instruction.
 * @offset: Offset of the instruction in the chunk's code array.
 *
 * This function prints the instruction's name, constant index, and value.
 * It returns the offset of the next instruction.
 *
 * Return: The offset of the next instruction.
 */
static int constant_instruction(const char *name, chunk_t *chunk, int offset)
{
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    print_value(chunk->constants.values[constant]);
    printf("'\n");
    return (offset + 2);
}
