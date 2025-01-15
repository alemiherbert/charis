/**
 * charis - a graceful little scripting language, based on clox
 * author: Alemi Herbert <alemiherbert@gmail.com>
 * (c) Alemi Herbert 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "debug.h"
#include "chunk.h"
#include "value.h"
#include "vm.h"

/**
 * main - the entry point to the program
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 on success
 */
int main(int argc, char *argv[])
{
	init_vm();
	chunk_t chunk;
	init_chunk(&chunk);

	int constant = add_constant(&chunk, 1.2);
	write_chunk(&chunk, OP_CONSTANT, 123);
	write_chunk(&chunk, constant, 123);

	constant = add_constant(&chunk, 3.4);
	write_chunk(&chunk, OP_CONSTANT, 123);
	write_chunk(&chunk, constant, 123);

	write_chunk(&chunk, OP_ADD, 123);

	constant = add_constant(&chunk, 5.6);
	write_chunk(&chunk, OP_CONSTANT, 123);
	write_chunk(&chunk, constant, 123);

	write_chunk(&chunk, OP_DIVIDE, 123);

	write_chunk(&chunk, OP_NEGATE, 123);
	write_chunk(&chunk, OP_RETURN, 123);

	//disassemble_chunk(&chunk, "test chunk");
	interpret(&chunk);
	free_vm();
	free_chunk(&chunk);
	return (0);
}
