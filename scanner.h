#pragma once

/**
 * struct scanner_s - scanner object for tokenizing.
 * @start: pointer to start of the current scanning segment.
 * @current: pointer to the current position in the input
 * @line: the current line number
 */
typedef struct scanner_s
{
    const char *start;
    const char *current;
    int line;
} scanner_t;

void init_scanner(const char *source);