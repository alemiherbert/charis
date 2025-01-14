#pragma once
#include <stdlib.h>
#include "common.h"

size_t grow_capacity(size_t capacity);
void *grow_array(void* pointer, size_t old_count, size_t new_count, size_t element_size);
void* reallocate(void* pointer, size_t old_size, size_t new_size);
void free_array(void* pointer);