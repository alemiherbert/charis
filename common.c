#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "vm.h"

/**
 * run_file - run a source file
 * @path: path to the source code
 */
#if 0
static void run_file(const char *path)
{
	char *source = read_file(path);
	interpret_result_t result = interpret(source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR)
		exit(65);
	if (result == INTERPRET_RUNTIME_ERROR)
		exit(70);
}
#endif

/**
 * read_file - read a source file
 * @path: path to the file to read
 * Return: pointer to the characters read
 */
static char *read_file(const char *path)
{
    // Attempt to open file
	FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file '%s'.\n", path);
        exit(74);
    }

    // Determine the file size
	fseek(file, 0L, SEEK_END);
	size_t file_size = ftell(file);
	rewind(file);

    // Allocate memory for file contents
	char *buffer = (char *) malloc(file_size + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read '%s'.\n", path);
        fclose(file);
        exit(74);
    }

    // Read file into the bffer
	size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size)
    {
        fprintf(stderr, "Failed to read the entire file.\n");
        fclose(file);
        return (NULL);
    }

	buffer[bytes_read] = '\0';

	fclose(file);
	return (buffer);
}
