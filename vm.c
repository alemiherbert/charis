#include <stdbool.h>
#include "compiler.h"
#include "vm.h"

vm_t vm;

void init_vm(void)
{
	reset_stack();
}

void free_vm(void)
{
	if (vm.stack != NULL)
	{
		free(vm.stack);
	}
	vm.stack = NULL;
	vm.stack_top = NULL;
	vm.stack_capacity = 0;
	vm.ip = NULL;
	vm.chunk = NULL;
}

interpret_result_t interpret(const char *source)
{
	compile(source);
	return (INTERPRET_OK);
}

static void handle_OP_CONSTANT(void)
{
	uint8_t constant_idx = *vm.ip++;
	value_t constant = vm.chunk->constants.values[constant_idx];
	push(constant);
}

static void handle_OP_NEGATE(void)
{
	*(vm.stack_top - 1) = -*(vm.stack_top - 1);
}

static void handle_OP_ADD(void)
{
	*(vm.stack_top - 2) = *(vm.stack_top - 2) + *(vm.stack_top - 1);
	vm.stack_top--;
}

static void handle_OP_SUBTRACT(void)
{
	*(vm.stack_top - 2) = *(vm.stack_top - 2) - *(vm.stack_top - 1);
	vm.stack_top--;
}

static void handle_OP_MULTIPLY(void)
{
	*(vm.stack_top - 2) = *(vm.stack_top - 2) * *(vm.stack_top - 1);
	vm.stack_top--;
}

static void handle_OP_DIVIDE(void)
{
	*(vm.stack_top - 2) = *(vm.stack_top - 2) / *(vm.stack_top - 1);
	vm.stack_top--;
}

static interpret_result_t handle_OP_RETURN(void)
{
	print_value(pop());
	printf("\n");
	return (INTERPRET_OK);
}

static instruction_handler_t jump_table[] = {
	[OP_CONSTANT] = handle_OP_CONSTANT,
	[OP_NEGATE] = handle_OP_NEGATE,
	[OP_ADD] = handle_OP_ADD,
	[OP_SUBTRACT] = handle_OP_SUBTRACT,
	[OP_MULTIPLY] = handle_OP_MULTIPLY,
	[OP_DIVIDE] = handle_OP_DIVIDE,
	[OP_RETURN] = (instruction_handler_t)handle_OP_RETURN,
};

static interpret_result_t run(void)
{
	while (true)
	{
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (value_t *slot = vm.stack; slot < vm.stack_top; slot++)
		{
			printf("[ ");
			print_value(*slot);
			printf(" ]");
		}
		printf("\n");
		disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
		uint8_t instruction = *vm.ip++;
		instruction_handler_t handler = jump_table[instruction];
		if (instruction == OP_RETURN)
		{
			return handle_OP_RETURN();
		}
		handler();
	}
}

void reset_stack(void)
{
	if (vm.stack != NULL)
	{
		free(vm.stack);
	}
	vm.stack = NULL;
	vm.stack_top = NULL;
	vm.stack_capacity = 0;
	grow_stack();
	vm.stack_top = vm.stack;
	vm.ip = NULL;
	vm.chunk = NULL;
}

static void grow_stack(void)
{
	size_t new_capacity = vm.stack_capacity == 0 ? 256 : vm.stack_capacity * 2;
	value_t *new_stack = (value_t *)realloc(vm.stack, new_capacity * sizeof(value_t));
	if (new_stack == NULL)
	{
		fprintf(stderr, "Error: Failed to grow stack\n");
		exit(INTERPRET_RUNTIME_ERROR);
	}
	vm.stack = new_stack;
	vm.stack_top = vm.stack + (vm.stack_top - vm.stack);
	vm.stack_capacity = new_capacity;
}

void push(value_t value)
{
	if (vm.stack_top - vm.stack >= vm.stack_capacity)
		grow_stack();
	*vm.stack_top++ = value;
}

value_t pop(void)
{
	return (*--vm.stack_top);
}