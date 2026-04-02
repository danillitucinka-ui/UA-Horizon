#include "../../include/stdio.h"
#include <stdarg.h>

static char *video_memory = (char *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;

static void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= 25) cursor_y = 24;
        return;
    }
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    video_memory[(cursor_y * 80 + cursor_x) * 2] = c;
    video_memory[(cursor_y * 80 + cursor_x) * 2 + 1] = 0x07;
    cursor_x++;
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
}

static void print_hex(uint32_t val) {
    const char *hex = "0123456789ABCDEF";
    putchar('0');
    putchar('x');
    for (int i = 7; i >= 0; i--) {
        putchar(hex[(val >> (i * 4)) & 0xF]);
    }
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    const char *p = format;
    while (*p) {
        if (*p == '%' && *(p + 1)) {
            p++;
            switch (*p) {
                case 'd': {
                    int val = va_arg(args, int);
                    char buf[12];
                    int i = 0;
                    if (val < 0) { putchar('-'); val = -val; }
                    if (val == 0) { putchar('0'); }
                    else {
                        while (val > 0) { buf[i++] = '0' + (val % 10); val /= 10; }
                        while (i > 0) { putchar(buf[--i]); }
                    }
                    break;
                }
                case 'x':
                case 'p': {
                    uint32_t val = va_arg(args, uint32_t);
                    const char *hex = "0123456789abcdef";
                    for (int i = 7; i >= 0; i--) {
                        putchar(hex[(val >> (i * 4)) & 0xF]);
                    }
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char *);
                    while (*s) { putchar(*s++); }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    putchar(c);
                    break;
                }
                case '%':
                    putchar('%');
                    break;
                default:
                    putchar('%');
                    putchar(*p);
                    break;
            }
        } else {
            putchar(*p);
        }
        p++;
    }
    va_end(args);
    return 0;
}

void terminal_init(void) {
    cursor_x = 0;
    cursor_y = 0;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x07;
    }
}

void sleep(uint32_t ms) {
    volatile uint32_t count = ms * 100000;
    while (count--) { __asm__ volatile ("nop"); }
}