#ifndef STRING_H
#define STRING_H

#include "stdint.h"
#include <stddef.h>

// NULL definition
#define NULL ((void *)0)

// String manipulation functions
void copy_str(char *dest, const char *src);
void concat_str(char *dest, const char *src);
int compare_str(const char *s1, const char *s2);
int strcmp(const char *s1, const char *s2); // Alias for compare_str
int string_length(const char *str);
void *memory_set(void *ptr, int value, size_t size);
void *memory_copy(void *dest, const void *src, size_t size);

#endif