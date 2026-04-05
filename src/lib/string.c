#include "../include/string.h"
#include <stdint.h>

// Copy string from src to dest
char *copy_str(char *dest, const char *src) {
    if (!dest || !src) return dest;
    char *original_dest = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

// Concatenate src to the end of dest
char *concat_str(char *dest, const char *src) {
    if (!dest || !src) return dest;
    char *original_dest = dest;
    // Find end of dest
    while (*dest) dest++;
    // Copy src
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

// Compare two strings, return 0 if equal
int compare_str(const char *s1, const char *s2) {
    if (!s1 || !s2) return -1;
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// strcmp is an alias for compare_str
int strcmp(const char *s1, const char *s2) {
    return compare_str(s1, s2);
}

// Compare up to n characters
int strncmp(const char *s1, const char *s2, size_t n) {
    if (!s1 || !s2 || n == 0) return 0;

    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

// Copy string (same as copy_str)
char *string_copy(char *dest, const char *src) {
    return (char *)copy_str(dest, src);
}

// Concatenate strings (same as concat_str)
char *string_cat(char *dest, const char *src) {
    return (char *)concat_str(dest, src);
}

// Find character in string
char *string_chr(const char *str, int c) {
    if (!str) return NULL;

    while (*str) {
        if (*str == (char)c) {
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

// Find substring in string
char *string_str(const char *haystack, const char *needle) {
    if (!haystack || !needle) return NULL;
    if (*needle == '\0') return (char *)haystack;

    for (const char *h = haystack; *h; h++) {
        const char *h_ptr = h;
        const char *n_ptr = needle;

        while (*h_ptr && *n_ptr && *h_ptr == *n_ptr) {
            h_ptr++;
            n_ptr++;
        }

        if (*n_ptr == '\0') {
            return (char *)h;
        }
    }

    return NULL;
}

// Get string length
size_t string_length(const char *str) {
    if (!str) return 0;
    size_t len = 0;
    while (*str++) len++;
    return len;
}

// Memory set (same as memset)
void *memory_set(void *ptr, int value, size_t size) {
    unsigned char *p = (unsigned char *)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

// Memory copy (same as memcpy)
void *memory_copy(void *dest, const void *src, size_t size) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    
    if (((uintptr_t)d & 3) == 0 && ((uintptr_t)s & 3) == 0) {
        uint32_t *d32 = (uint32_t *)d;
        const uint32_t *s32 = (const uint32_t *)s;
        
        while (size >= 4) {
            *d32++ = *s32++;
            d += 4;
            s += 4;
            size -= 4;
        }
        
        d = (unsigned char *)d32;
        s = (const unsigned char *)s32;
    }
    
    while (size--) {
        *d++ = *s++;
    }
    return dest;
}

// Simple atoi implementation
int string_to_int(const char *str) {
    if (!str) return 0;

    int result = 0;
    int sign = 1;

    // Skip whitespace
    while (*str == ' ' || *str == '\t') str++;

    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

// Simple atof implementation (basic float conversion)
double string_to_float(const char *str) {
    if (!str) return 0.0;

    double result = 0.0;
    int sign = 1;
    double fraction = 0.0;
    int decimal_seen = 0;
    double divisor = 1.0;

    // Skip whitespace
    while (*str == ' ' || *str == '\t') str++;

    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Convert digits
    while (*str) {
        if (*str == '.') {
            decimal_seen = 1;
            str++;
            continue;
        }

        if (*str >= '0' && *str <= '9') {
            if (decimal_seen) {
                divisor *= 10.0;
                fraction = fraction + (*str - '0') / divisor;
            } else {
                result = result * 10.0 + (*str - '0');
            }
        } else {
            break;
        }
        str++;
    }

    return sign * (result + fraction);
}

// Very basic sprintf implementation (only %d, %s supported)
int string_printf(char *buffer, size_t size, const char *format, ...) {
    // Very basic implementation - just copy the format string
    // In a real implementation, this would parse format specifiers
    if (!buffer || !format || size == 0) return 0;

    size_t len = 0;
    while (*format && len < size - 1) {
        buffer[len++] = *format++;
    }
    buffer[len] = '\0';
    return len;
}

// Very basic sscanf implementation (only %s supported)
int string_scanf(const char *str, const char *format, ...) {
    // This is a very basic implementation - only supports %s
    // In a real implementation, this would be much more complex
    (void)str;
    (void)format;
    return 0; // Not implemented
}