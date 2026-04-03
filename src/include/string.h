#ifndef STRING_H
#define STRING_H

#include "stdint.h"
#include <stddef.h>

// NULL definition
#define NULL ((void *)0)

// String manipulation functions
char *copy_str(char *dest, const char *src);
char *concat_str(char *dest, const char *src);
int compare_str(const char *s1, const char *s2);
int strcmp(const char *s1, const char *s2); // Alias for compare_str
int strncmp(const char *s1, const char *s2, size_t n);
int string_length(const char *str);
char *string_copy(char *dest, const char *src);
char *string_cat(char *dest, const char *src);
char *string_chr(const char *str, int c);
char *string_str(const char *haystack, const char *needle);
int string_to_int(const char *str);
int string_scanf(const char *str, const char *format, ...);
void *memory_set(void *ptr, int value, size_t size);
void *memory_copy(void *dest, const void *src, size_t size);

// Standard C library compatibility (macros)
#define strcpy copy_str
#define strcat concat_str
#define strchr string_chr
#define strstr string_str
#define strlen string_length
#define atoi string_to_int
#define sscanf string_scanf

#endif