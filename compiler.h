#pragma once
#ifndef COMPILER_H
#define COMPILER_H
#include "common.h"
#include "chunk.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

/**
 * struct parser_s - Represents the state of the parser.
 * @current: The current token being processed.
 * @previous: The previous token that was processed.
 * @had_error: Flag indicating whether an error has occurred during parsing.
 * @panic_mode: Flag indicating whether the parser is in panic mode (error recovery).
 *
 * Description: This structure holds the state of the parser, including the current and previous
 * tokens, as well as flags for error handling and recovery.
 */
typedef struct parser_s
{
    token_t current;
    token_t previous;
    bool had_error;
    bool panic_mode;
} parser_t;

/**
 * enum precedence_s - Represents the precedence levels for parsing expressions.
 * @PREC_NONE: No precedence (lowest).
 * @PREC_ASSIGNMENT: Precedence for assignment operators (=).
 * @PREC_TERNARY: Precedence for ternary operator (?:)
 * @PREC_OR: Precedence for logical OR operators (or).
 * @PREC_AND: Precedence for logical AND operators (and).
 * @PREC_EQUALITY: Precedence for equality operators (==, !=).
 * @PREC_COMPARISON: Precedence for comparison operators (<, >, <=, >=).
 * @PREC_TERM: Precedence for term operators (+, -).
 * @PREC_FACTOR: Precedence for factor operators (*, /).
 * @PREC_UNARY: Precedence for unary operators (!, -).
 * @PREC_CALL: Precedence for function calls and member access (., ()).
 * @PREC_PRIMARY: Highest precedence for primary expressions (literals, identifiers, etc.).
 *
 * Description: This enumeration defines the precedence levels used to parse expressions
 * in the correct order of operations.
 */
typedef enum precedence_s
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_TERNARY,    // ?:
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} precedence_t;

typedef void (*parse_fn)(void);

/**
 * struct parse_rule_s - Represents a parsing rule for a token type.
 * @prefix: Function pointer to the prefix parsing function for the token.
 * @infix: Function pointer to the infix parsing function for the token.
 * @precedence: The precedence level for the token.
 *
 * Description: This structure defines a parsing rule for a specific token type,
 * including its prefix and infix parsing functions and its precedence level.
 */
typedef struct parse_rule_s
{
    parse_fn prefix;
    parse_fn infix;
    precedence_t precedence;
} parse_rule_t;

bool compile(const char *source, chunk_t *chunk);

#endif // COMPILER_H
