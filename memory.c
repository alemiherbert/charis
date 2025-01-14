#include <stdlib.h>
#include "memory.h"

size_t grow_capacity(size_t capacity)
{
	return (capacity < 8) ? 8 : capacity * 2;
}

void *grow_array(void *pointer, size_t old_count, size_t new_count, size_t element_size)
{
	return reallocate(pointer, element_size * old_count, element_size * new_count);
}

void *reallocate(void *pointer, size_t old_size, size_t new_size)
{
	if (new_size == 0)
	{
		free(pointer);
		return (NULL);
	}

	void *result = realloc(pointer, new_size);
	if (result == NULL)
	{
		exit(1);
	}
	return (result);
}

void free_array(void* pointer) {
	if (pointer != NULL)
    	reallocate(pointer, 0, 0);
}