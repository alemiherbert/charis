#include "compiler.h"

parser_t parser;
chunk_t *compiling_chunk;

static void literal(void);
static void unary(void);
static void binary(void);
static void ternary(void);
static void grouping(void);
static void number(void);

parse_rule_t rules[] = {
	[TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
	[TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
	[TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
	[TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
	[TOKEN_COLON] = {NULL, NULL, PREC_NONE},
	[TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
	[TOKEN_CONST] = {NULL, NULL, PREC_NONE},
	[TOKEN_DEFINE] = {NULL, NULL, PREC_NONE},
	[TOKEN_DOT] = {NULL, NULL, PREC_NONE},
	[TOKEN_LET] = {NULL, NULL, PREC_NONE},
	[TOKEN_MINUS] = {unary, binary, PREC_TERM},
	[TOKEN_PLUS] = {unary, binary, PREC_TERM},
	[TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
	[TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
	[TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
	[TOKEN_BANG] = {unary, NULL, PREC_NONE},
	[TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
	[TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
	[TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
	[TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
	[TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
	[TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
	[TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
	[TOKEN_STRING] = {NULL, NULL, PREC_NONE},
	[TOKEN_QUESTION] = {NULL, ternary, PREC_TERNARY},
	[TOKEN_NUMBER] = {number, NULL, PREC_NONE},
	[TOKEN_AND] = {NULL, NULL, PREC_NONE},
	[TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
	[TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
	[TOKEN_FALSE] = {literal, NULL, PREC_NONE},
	[TOKEN_FOR] = {NULL, NULL, PREC_NONE},
	[TOKEN_IF] = {NULL, NULL, PREC_NONE},
	[TOKEN_NULL] = {literal, NULL, PREC_NONE},
	[TOKEN_OR] = {NULL, NULL, PREC_NONE},
	[TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
	[TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
	[TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
	[TOKEN_THIS] = {NULL, NULL, PREC_NONE},
	[TOKEN_TRUE] = {literal, NULL, PREC_NONE},
	[TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
	[TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
	[TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

// Returns the current chunk being compiled.
static chunk_t *current_chunk(void) { return compiling_chunk; }

// Reports an error at a specific token.
static void error_at(token_t *token, const char *message)
{
	if (parser.panic_mode)
		return;
	parser.panic_mode = true;
	fprintf(stderr, "[line %d] Error", token->line);
	if (token->type == TOKEN_EOF)
		fprintf(stderr, " at end");
	else if (token->type != TOKEN_ERROR)
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	fprintf(stderr, " at '%s\n", message);
	parser.had_error = true;
}

// Reports an error at the current token.
static void error_at_current(const char *message) { error_at(&parser.current, message); }

// Reports an error at the previous token.
static void error(const char *message) { error_at(&parser.previous, message); }

// Advances the parser to the next token.
static void advance(void)
{
	parser.previous = parser.current;
	while (true)
	{
		parser.current = scan_token();
		if (parser.current.type != TOKEN_ERROR)
			break;
		error_at_current(parser.current.start);
	}
}

// Consumes a token of the expected type or reports an error.
static void consume(token_type_t type, const char *message)
{
	if (parser.current.type == type)
	{
		advance();
		return;
	}
	error_at_current(message);
}

// Emits a byte of bytecode.
static void emit_byte(uint8_t byte) { write_chunk(current_chunk(), byte, parser.previous.line); }

// Emits two bytes of bytecode.
static void emit_bytes(uint8_t byte1, uint8_t byte2)
{
	emit_byte(byte1);
	emit_byte(byte2);
}

// Adds a constant value to the chunk and returns its index.
static uint8_t make_constant(value_t value)
{
	int constant = add_constant(current_chunk(), value);
	if (constant > UINT8_MAX)
	{
		error("Too many constants in one chunk");
		return 0;
	}
	return (uint8_t)constant;
}

// Emits a constant instruction.
static void emit_constant(value_t value) { emit_bytes(OP_CONSTANT, make_constant(value)); }

// Emits a return instruction.
static void emit_return(void) { emit_byte(OP_RETURN); }

// Finalizes the compilation process.
static void end_compiler(void)
{
	emit_return();
#ifdef DEBUG_PRINT_CODE
	if (!parser.had_error)
		disassemble_chunk(current_chunk(), "code");
#endif
}

// Retrieves the parsing rule for a token type.
static parse_rule_t *get_rule(token_type_t type) { return &rules[type]; }

// Parses an expression with a given precedence level.
static void parse_precedence(precedence_t precedence)
{
	advance();
	parse_fn prefix_rule = get_rule(parser.previous.type)->prefix;
	if (prefix_rule == NULL)
	{
		error("Expect expression.");
		return;
	}
	prefix_rule();
	while (precedence <= get_rule(parser.current.type)->precedence)
	{
		advance();
		parse_fn infix_rule = get_rule(parser.previous.type)->infix;
		infix_rule();
	}
}

// Parses an expression starting with the lowest precedence.
static void expression(void) { parse_precedence(PREC_ASSIGNMENT); }

// Parses and emits bytecode for a binary expression.
static void binary(void)
{
	token_type_t operator_type = parser.previous.type;
	parse_rule_t *rule = get_rule(operator_type);
	parse_precedence((precedence_t)(rule->precedence + 1));
	switch (operator_type)
	{
	case TOKEN_PLUS:
		emit_byte(OP_ADD);
		break;
	case TOKEN_MINUS:
		emit_byte(OP_SUBTRACT);
		break;
	case TOKEN_STAR:
		emit_byte(OP_MULTIPLY);
		break;
	case TOKEN_SLASH:
		emit_byte(OP_DIVIDE);
		break;
	case TOKEN_BANG_EQUAL:
		emit_bytes(OP_EQUAL, OP_NOT);
		break;
	case TOKEN_EQUAL_EQUAL:
		emit_byte(OP_EQUAL);
		break;
	case TOKEN_GREATER:
		emit_byte(OP_GREATER);
		break;
	case TOKEN_GREATER_EQUAL:
		emit_bytes(OP_LESS, OP_NOT);
		break;
	case TOKEN_LESS:
		emit_byte(OP_LESS);
		break;
	case TOKEN_LESS_EQUAL:
		emit_bytes(OP_GREATER, OP_NOT);
		break;
	default:
		return;
	}
}

// Parses a grouping expression (enclosed in parentheses).
static void grouping(void)
{
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void literal(void)
{
	switch (parser.previous.type)
	{
	case TOKEN_FALSE:
		emit_byte(OP_FALSE);
		break;
	case TOKEN_NULL:
		emit_byte(OP_NULL);
		break;
	case TOKEN_TRUE:
		emit_byte(OP_TRUE);
		break;
	default:
		return;
	}
}

// Parses and emits bytecode for a numeric literal.
static void number(void)
{
	double value = strtod(parser.previous.start, NULL);
	emit_constant(number_val(value));
}

// Parses and emits bytecode for a unary expression.
static void unary(void)
{
	token_type_t operator_type = parser.previous.type;
	parse_precedence(PREC_UNARY);
	switch (operator_type)
	{
	case TOKEN_MINUS:
		emit_byte(OP_NEGATE);
		break;
	case TOKEN_PLUS:
		break;
	case TOKEN_BANG:
		emit_byte(OP_NOT);
		break;
	default:
		return;
	}
}

// Parses and emits bytecode for a ternary expression
static void ternary(void)
{
	parse_precedence(PREC_TERNARY + 1); // Higher precedence than ?:
	consume(TOKEN_COLON, "Expect ':' after then branch of ternary expression.");
	parse_precedence(PREC_TERNARY);
}

// Compiles source code into bytecode.
bool compile(const char *source, chunk_t *chunk)
{
	init_scanner(source);
	compiling_chunk = chunk;
	parser.had_error = false;
	parser.panic_mode = false;
	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");
	end_compiler();
	return !parser.had_error;
}