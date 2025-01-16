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


