#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

#define NULL ((void *)0)

void terminal_init(void);
int printf(const char *format, ...);
void fs_init(void);
void memory_init(void);
void pit_init(void);
void keyboard_init(void);
void keyboard_poll(void);
void vbe_init(void);
void gui_init(void);
void sound_init(void);
void usb_init(void);
void sleep(uint32_t ms);

#endif