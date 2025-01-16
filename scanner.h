#pragma once
#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

// Token types
typedef enum
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_HASH,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,
    TOKEN_QUESTION,

    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_CONST,
    TOKEN_DEFINE,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_LET,
    TOKEN_NULL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_SUPER,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_WHILE,

    TOKEN_ERROR,
    TOKEN_EOF
} token_type_t;

/**
 * struct token_s - Represents a token in the source code.
 * @type: The type of the token (e.g., TOKEN_IDENTIFIER, TOKEN_NUMBER, etc.).
 * @start: Pointer to the start of the token in the source code.
 * @length: The length of the token in characters.
 * @line: The line number in the source code where the token appears.
 *
 * Description: This structure holds information about a token, including its type,
 * location in the source code, length, and the line number where it was found.
 */
typedef struct token_s
{
    token_type_t type;
    const char *start;
    int length;
    int line;
} token_t;

/**
 * struct scanner_s - Represents the state of the scanner.
 * @start: Pointer to the start of the current token being scanned.
 * @current: Pointer to the current position in the source code.
 * @line: The current line number in the source code.
 *
 * Description: This structure holds the state of the scanner, including the current
 * position in the source code, the start of the current token, and the current line number.
 */
typedef struct scanner_s
{
    const char *start;
    const char *current;
    int line;
} scanner_t;

// Function prototypes
void init_scanner(const char *source);
token_t scan_token(void);

#endif // SCANNER_H