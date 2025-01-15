#pragma once

#include <stdio.h>
#include <stdlib.h>

#define DEBUG_TRACE_EXECUTION

static void run_file(const char *path);
static char *read_file(const char *path);