#include <stdio.h>
#include <string.h>
#include "common.h"
#include "scanner.h"

scanner_t scanner;

// Initializes the scanner with the source code.
void init_scanner(const char *source)
{
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

// Checks if a character is a digit.
static bool is_digit(char c) { return (c >= '0' && c <= '9'); }

// Checks if a character is alphabetic or underscore.
static bool is_alpha(char c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'); }

// Checks if the scanner has reached the end of the source.
static bool is_at_end(void) { return (*scanner.current == '\0'); }

// Advances the scanner and returns the current character.
static char advance(void) { return *scanner.current++; }

// Matches the current character with the expected one.
static bool match(char expected)
{
	if (is_at_end() || *scanner.current != expected)
		return false;
	scanner.current++;
	return true;
}

// Returns the current character without advancing.
static char peek(void) { return *scanner.current; }

// Returns the next character without advancing.
static char peek_next(void)
{
	if (is_at_end())
		return '\0';
	return scanner.current[1];
}

// Skips whitespace and comments.
static void skip_whitespace(void)
{
	while (true)
	{
		char c = peek();
		switch (c)
		{
		case ' ':
		case '\r':
		case '\t':
			advance();
			break;
		case '\n':
			scanner.line++;
			advance();
			break;
		case '#':
			// Comments go until the end of the line.
			while (peek() != '\n' && !is_at_end())
				advance();
			break;
		default:
			return;
		}
	}
}

// Creates a token of the given type.
static token_t make_token(token_type_t type)
{
	token_t token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

// Creates an error token with the given message.
static token_t error_token(const char *message)
{
	token_t token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

// Scans a string literal.
static token_t string(void)
{
	while (peek() != '"' && !is_at_end())
	{
		if (peek() == '\n')
			scanner.line++;
		advance();
	}
	if (is_at_end())
		return error_token("Unterminated string.");
	advance();
	return make_token(TOKEN_STRING);
}

// Scans a numeric literal.
static token_t number(void)
{
	while (is_digit(peek()))
		advance();
	if (peek() == '.' && is_digit(peek_next()))
	{
		advance();
		while (is_digit(peek()))
			advance();
	}
	return make_token(TOKEN_NUMBER);
}

// Checks if a keyword matches.
static token_type_t check_keyword(int start, int length, const char *rest, token_type_t type)
{
	if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0)
	{
		return type;
	}
	return TOKEN_IDENTIFIER;
}

// Determines the type of an identifier or keyword.
static token_type_t identifier_type(void)
{
	switch (scanner.start[0])
	{
	case 'a':
		return check_keyword(1, 2, "nd", TOKEN_AND);
	case 'c':
		if (scanner.current - scanner.start > 1)
		{
			switch (scanner.start[1])
			{
			case 'l':
				return check_keyword(2, 3, "ass", TOKEN_CLASS);
			case 'o':
				return check_keyword(2, 3, "nst", TOKEN_CONST);
			}
		}
		break;
	case 'd':
		return check_keyword(1, 5, "efine", TOKEN_DEFINE);
	case 'e':
		return check_keyword(1, 3, "lse", TOKEN_ELSE);
	case 'f':
		if (scanner.current - scanner.start > 1)
		{
			switch (scanner.start[1])
			{
			case 'a':
				return check_keyword(2, 3, "lse", TOKEN_FALSE);
			case 'o':
				return check_keyword(2, 1, "r", TOKEN_FOR);
			}
		}
		break;
	case 'i':
		return check_keyword(1, 1, "f", TOKEN_IF);
	case 'l':
		return check_keyword(1, 2, "et", TOKEN_LET);
	case 'n':
		return check_keyword(1, 3, "ull", TOKEN_NULL);
	case 'o':
		return check_keyword(1, 1, "r", TOKEN_OR);
	case 'p':
		return check_keyword(1, 4, "rint", TOKEN_PRINT);
	case 'r':
		return check_keyword(1, 5, "eturn", TOKEN_RETURN);
	case 's':
		return check_keyword(1, 4, "uper", TOKEN_SUPER);
	case 't':
		if (scanner.current - scanner.start > 1)
		{
			switch (scanner.start[1])
			{
			case 'h':
				return check_keyword(2, 2, "is", TOKEN_THIS);
			case 'r':
				return check_keyword(2, 2, "ue", TOKEN_TRUE);
			}
		}
		break;
	case 'w':
		return check_keyword(1, 4, "hile", TOKEN_WHILE);
	}
	return TOKEN_IDENTIFIER;
}

static token_t identifier(void)
{ // Scans an identifier or keyword.
	while (is_alpha(peek()) || is_digit(peek()))
		advance();
	return make_token(identifier_type());
}

token_t scan_token(void)
{ // Scans and returns the next token.
	skip_whitespace();
	scanner.start = scanner.current;

	if (is_at_end())
		return make_token(TOKEN_EOF);

	char c = advance();
	if (is_digit(c))
		return number();
	if (is_alpha(c))
		return identifier();

	switch (c)
	{
	case '(':
		return make_token(TOKEN_LEFT_PAREN);
	case ')':
		return make_token(TOKEN_RIGHT_PAREN);
	case '{':
		return make_token(TOKEN_LEFT_BRACE);
	case '}':
		return make_token(TOKEN_RIGHT_BRACE);
	case '[':
		return make_token(TOKEN_LEFT_BRACKET);
	case ']':
		return make_token(TOKEN_RIGHT_BRACKET);

	case ':':
		return make_token(TOKEN_COLON);
	case ';':
		return make_token(TOKEN_SEMICOLON);
	case ',':
		return make_token(TOKEN_COMMA);
	case '.':
		return make_token(TOKEN_DOT);
	case '#':
		return make_token(TOKEN_HASH);
	case '?':
		return make_token(TOKEN_QUESTION);

	case '-':
		return make_token(TOKEN_MINUS);
	case '+':
		return make_token(TOKEN_PLUS);
	case '/':
		return make_token(TOKEN_SLASH);
	case '*':
		return make_token(TOKEN_STAR);

	case '!':
		return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
	case '=':
		return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
	case '<':
		return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
	case '>':
		return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

	// String literal
	case '"':
		return string();
	}

	return error_token("Unexpected Character");
}
