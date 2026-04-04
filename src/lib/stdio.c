#include "../include/stdio.h"
#include "../include/stdint.h"
#include <stdarg.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (char *)0xB8000

static char *video_memory = VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;

static void video_scroll(void) {
    if (cursor_y >= VGA_HEIGHT) {
        int scroll_lines = cursor_y - VGA_HEIGHT + 1;
        for (int y = 0; y < VGA_HEIGHT - scroll_lines; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                video_memory[(y * VGA_WIDTH + x) * 2] = 
                    video_memory[((y + scroll_lines) * VGA_WIDTH + x) * 2];
                video_memory[(y * VGA_WIDTH + x) * 2 + 1] = 
                    video_memory[((y + scroll_lines) * VGA_WIDTH + x) * 2 + 1];
            }
        }
        // Clear bottom lines
        for (int y = VGA_HEIGHT - scroll_lines; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                video_memory[(y * VGA_WIDTH + x) * 2] = ' ';
                video_memory[(y * VGA_WIDTH + x) * 2 + 1] = 0x07;
            }
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

static void video_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        video_scroll();
        return;
    }
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
            video_scroll();
        }
        return;
    }
    if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = ' ';
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = VGA_WIDTH - 1;
        }
        return;
    }
    video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = c;
    video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2 + 1] = 0x07;
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        video_scroll();
    }
}

static void print_int(int val) {
    char buf[12];
    int i = 0;
    int is_negative = val < 0;
    
    if (is_negative) {
        video_putchar('-');
        if (val == (int)0x80000000) {
            video_putchar('2');
            val = 147483648;
        } else {
            val = -val;
        }
    }
    
    if (val == 0) {
        video_putchar('0');
        return;
    }
    
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        video_putchar(buf[--i]);
    }
}

static void print_uint(uint32_t val) {
    char buf[11];
    int i = 0;
    
    if (val == 0) {
        video_putchar('0');
        return;
    }
    
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        video_putchar(buf[--i]);
    }
}

static void print_hex(uint32_t val, int prefix) {
    const char *hex = "0123456789abcdef";
    int started = 0;
    
    if (prefix) {
        video_putchar('0');
        video_putchar('x');
    }
    
    for (int i = 7; i >= 0; i--) {
        int digit = (val >> (i * 4)) & 0xF;
        if (digit != 0 || i == 0 || started) {
            video_putchar(hex[digit]);
            started = 1;
        }
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
                case 'd':
                case 'i': {
                    int val = va_arg(args, int);
                    print_int(val);
                    break;
                }
                case 'u': {
                    uint32_t val = va_arg(args, uint32_t);
                    print_uint(val);
                    break;
                }
                case 'x': {
                    uint32_t val = va_arg(args, uint32_t);
                    print_hex(val, 0);
                    break;
                }
                case 'p': {
                    uint32_t val = va_arg(args, uint32_t);
                    print_hex(val, 1);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char *);
                    if (s) {
                        while (*s) {
                            video_putchar(*s++);
                        }
                    } else {
                        video_putchar('(');
                        video_putchar('n');
                        video_putchar('u');
                        video_putchar('l');
                        video_putchar('l');
                        video_putchar(')');
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    video_putchar(c);
                    break;
                }
                case '%':
                    video_putchar('%');
                    break;
                default:
                    video_putchar('%');
                    video_putchar(*p);
                    break;
            }
        } else {
            video_putchar(*p);
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