/**
 * charis - a graceful little scripting language, based on clox
 * author: Alemi Herbert <alemiherbert@gmail.com>
 * (c) Alemi Herbert 2024
 */

#include "common.h"
#include "debug.h"
#include "chunk.h"
#include "value.h"
#include "vm.h"

static void repl(void);
static void run_file(const char *path);
static char *read_file(const char *path);

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

	if (argc == 1)
	{
		repl();
	}
	else if (argc == 2)
	{
		run_file(argv[1]);
	}
	else
	{
		fprintf(stderr, "Usage: charis [path\n]");
	}

	free_vm();
	return (0);
}


static void repl(void) {
    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t line_length;

    printf("Welcome to charis 0.1 (type 'exit' to quit):\n");

    while (true) {
        printf(">> ");

        line_length = getline(&line, &buffer_size, stdin);

        if (line_length == -1) {
            printf("\n");
            break;
        }

        if (strcmp(line, "exit") == 0) {
            break;
        }

        interpret(line);
    }

    if (line != NULL) {
        free(line);
    }
}

/**
 * run_file - run a source file
 * @path: path to the source code
 */
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
