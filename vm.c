#include <stdarg.h>
#include "compiler.h"
#include "common.h"
#include "vm.h"

vm_t vm;

void init_vm(void) { reset_stack(); }

void free_vm(void)
{
	free(vm.stack);
	vm.stack = NULL;
	vm.stack_top = NULL;
	vm.stack_capacity = 0;
	vm.ip = NULL;
	vm.chunk = NULL;
}

interpret_result_t interpret(const char *source)
{
	chunk_t chunk;
	init_chunk(&chunk);

	if (!compile(source, &chunk))
	{
		free_chunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	interpret_result_t result = run();
	free_chunk(&chunk);
	return result;
}

static value_t peek(int distance) { return vm.stack_top[-1 - distance]; }

static void runtime_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = vm.chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);
	reset_stack();
}

static bool extract_binary_operands(double *a, double *b)
{
	if (!is_number(peek(0)) || !is_number(peek(1)))
	{
		runtime_error("Operands must be numbers.");
		return false;
	}

	*b = (vm.stack_top - 1)->as.number;
	*a = (vm.stack_top - 2)->as.number;
	return true;
}

static bool is_falsey(value_t value)
{
	return (is_null(value) || (is_bool(value) && !as_bool(value)));
}

static void handle_OP_CONSTANT(void)
{
	push(vm.chunk->constants.values[*vm.ip++]);
}

static void handle_OP_NULL(void)
{
	push(null_val());
}

static void handle_OP_TRUE(void)
{
	push(bool_val(true));
}

static void handle_OP_FALSE(void)
{
	push(bool_val(false));
}

static void handle_OP_NOT(void)
{
	value_t value = pop();
	push(bool_val(is_falsey(value)));
}

static void handle_OP_EQUAL(void) {
    value_t b = pop();
    value_t a = pop();

    if (is_bool(a)) a = number_val(as_bool(a) ? 1 : 0);
    if (is_bool(b)) b = number_val(as_bool(b) ? 1 : 0);

    if (a.type != b.type) {
        push(bool_val(false));
        return;
    }

    bool result = false;
    switch (a.type) {
        case VAL_NULL:
            result = true;
            break;
        case VAL_NUMBER:
            result = as_number(a) == as_number(b);
            break;
        default:
            result = false;
            break;
    }

    push(bool_val(result));
}

static void handle_OP_GREATER(void) {
    value_t b = pop();
    value_t a = pop();

    if (is_bool(a)) a = number_val(as_bool(a) ? 1 : 0);
    if (is_bool(b)) b = number_val(as_bool(b) ? 1 : 0);

    if (a.type != b.type) {
        runtime_error("Operands must be of the same type.");
        exit(EXIT_FAILURE);
    }

    bool result = false;
    switch (a.type) {
        case VAL_NUMBER:
            result = as_number(a) > as_number(b);
            break;
        default:
            runtime_error("Operands must be numbers.");
            exit(EXIT_FAILURE);
    }

    push(bool_val(result));
}

static void handle_OP_LESS(void) {
    value_t b = pop();
    value_t a = pop();

    if (is_bool(a)) a = number_val(as_bool(a) ? 1 : 0);
    if (is_bool(b)) b = number_val(as_bool(b) ? 1 : 0);

    if (a.type != b.type) {
        runtime_error("Operands must be of the same type.");
        exit(EXIT_FAILURE);
    }

    bool result = false;
    switch (a.type) {
        case VAL_NUMBER:
            result = as_number(a) < as_number(b);
            break;
        default:
            runtime_error("Operands must be numbers.");
            exit(EXIT_FAILURE);
    }

    push(bool_val(result));
}

static void handle_OP_NEGATE(void)
{
	if (!is_number(peek(0)))
	{
		runtime_error("Operand must be a number.");
		exit(EXIT_FAILURE);
	}
	(vm.stack_top - 1)->as.number = -(vm.stack_top - 1)->as.number;
}

static void handle_OP_ADD(void)
{
	double a, b;
	if (!extract_binary_operands(&a, &b))
		exit(EXIT_FAILURE);

	(vm.stack_top - 2)->as.number = a + b;
	vm.stack_top--;
}

static void handle_OP_SUBTRACT(void)
{
	double a, b;
	if (!extract_binary_operands(&a, &b))
		exit(EXIT_FAILURE);

	(vm.stack_top - 2)->as.number = a - b;
	vm.stack_top--;
}

static void handle_OP_MULTIPLY(void)
{
	double a, b;
	if (!extract_binary_operands(&a, &b))
		exit(EXIT_FAILURE);

	(vm.stack_top - 2)->as.number = a * b;
	vm.stack_top--;
}

static void handle_OP_DIVIDE(void)
{
	double a, b;
	if (!extract_binary_operands(&a, &b))
		exit(EXIT_FAILURE);

	(vm.stack_top - 2)->as.number = a / b;
	vm.stack_top--;
}

static interpret_result_t handle_OP_RETURN(void)
{
	print_value(pop());
	printf("\n");
	return INTERPRET_OK;
}

// Jump table for opcode handlers
static instruction_handler_t jump_table[] = {
	[OP_CONSTANT] = handle_OP_CONSTANT,
	[OP_NULL] = handle_OP_NULL,
	[OP_TRUE] = handle_OP_TRUE,
	[OP_FALSE] = handle_OP_FALSE,
	[OP_NOT] = handle_OP_NOT,
	[OP_EQUAL] = handle_OP_EQUAL,
	[OP_GREATER] = handle_OP_GREATER,
    [OP_LESS] = handle_OP_LESS,
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
	free(vm.stack);
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
	value_t *new_stack = realloc(vm.stack, new_capacity * sizeof(value_t));
	if (!new_stack)
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

value_t pop(void) { return *--vm.stack_top; }