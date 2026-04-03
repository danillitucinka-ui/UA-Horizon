#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

void memory_init(void);
void pit_init(void);
void pit_set_frequency(uint32_t frequency);
uint32_t pit_get_ticks(void);
void pit_tick(void);
void keyboard_init(void);
int keyboard_get_char(void);
void keyboard_poll(void);
void keyboard_set_leds(uint8_t leds);
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t value);

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint32_t bar[6];
} pci_device_t;

void pci_scan(void);
void pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint32_t *data);

typedef struct {
    uint32_t base;
    uint32_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    uint32_t *framebuffer;
} vbe_mode_info_t;

int vbe_get_mode_info(uint16_t mode, vbe_mode_info_t *info);
int vbe_set_mode(uint16_t mode);
void vbe_init(void);

void sound_beep(uint32_t freq, uint32_t duration_ms);

void terminal_init(void);
int printf(const char *format, ...);
void sleep(uint32_t ms);

#endif