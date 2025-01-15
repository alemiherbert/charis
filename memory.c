#include <stdlib.h>
#include "memory.h"

/**
 * grow_capacity - Calculates the new capacity for a dynamic array.
 * @capacity: The current capacity of the array.
 *
 * This function doubles the current capacity if it is greater than or equal to 8.
 * Otherwise, it returns a minimum capacity of 8.
 *
 * Return: The new capacity.
 */
size_t grow_capacity(size_t capacity)
{
	return (capacity < 8) ? 8 : capacity * 2;
}

/**
 * grow_array - Grows a dynamic array to a new size.
 * @pointer: Pointer to the current array.
 * @old_count: The current number of elements in the array.
 * @new_count: The desired number of elements in the array.
 * @element_size: The size of each element in the array.
 *
 * This function reallocates memory for the array to accommodate the new size.
 *
 * Return: A pointer to the reallocated array.
 */
void *grow_array(void *pointer, size_t old_count, size_t new_count, size_t element_size)
{
	return reallocate(pointer, element_size * old_count, element_size * new_count);
}

/**
 * reallocate - Reallocates memory for a pointer.
 * @pointer: Pointer to the memory block to reallocate.
 * @old_size: The current size of the memory block.
 * @new_size: The desired size of the memory block.
 *
 * This function reallocates memory for the given pointer. If `new_size` is 0,
 * it frees the memory block. Otherwise, it resizes the memory block to the
 * new size.
 *
 * Return: A pointer to the reallocated memory block, or NULL if `new_size` is 0.
 */
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

/**
 * free_array - Frees a dynamically allocated array.
 * @pointer: Pointer to the array to free.
 *
 * This function frees the memory allocated for the array and sets the pointer
 * to NULL.
 */
void free_array(void *pointer)
{
	if (pointer != NULL)
		reallocate(pointer, 0, 0);
}