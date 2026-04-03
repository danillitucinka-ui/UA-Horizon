#include "../include/string.h"

// Copy string from src to dest
void copy_str(char *dest, const char *src) {
    if (!dest || !src) return;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Concatenate src to the end of dest
void concat_str(char *dest, const char *src) {
    if (!dest || !src) return;
    // Find end of dest
    while (*dest) dest++;
    // Copy src
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
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

// Get string length
int string_length(const char *str) {
    if (!str) return 0;
    int len = 0;
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
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
    return dest;
}