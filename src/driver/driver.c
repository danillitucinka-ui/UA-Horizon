#include "../../include/driver.h"
#include "../../include/memory.h"

void memory_init(void) {
    pmm_init();
    vmm_init();
}

void pit_init(void) {
    outb(0x43, 0x36);
    outb(0x40, 0x00);
    outb(0x40, 0x00);
}

static uint8_t keyboard_buffer[256];
static int keyboard_read = 0;
static int keyboard_write = 0;

void keyboard_init(void) {
    // Initialize keyboard (enable interrupts, etc.)
    // For polling mode, just prepare
}

void keyboard_poll(void) {
    // Poll keyboard port for scancodes
    uint8_t status = inb(0x64);
    if (status & 0x01) {
        uint8_t scancode = inb(0x60);
        // Simple buffer
        if ((keyboard_write + 1) % 256 != keyboard_read) {
            keyboard_buffer[keyboard_write] = scancode;
            keyboard_write = (keyboard_write + 1) % 256;
        }
    }
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

void pci_scan(void) {}

void pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint32_t *data) {
    (void)bus; (void)slot; (void)func; (void)data;
}

int vbe_get_mode_info(uint16_t mode, vbe_mode_info_t *info) {
    (void)mode; (void)info;
    return 0;
}

int vbe_set_mode(uint16_t mode) {
    (void)mode;
    return 0;
}

void vbe_init(void) {
    // Initialize VESA graphics
    // In a real implementation, this would call BIOS int 10h to set mode
    // For now, assume a fixed framebuffer address
    // This is a stub since we're in long mode
    // gui_set_framebuffer((uint32_t *)0xFD000000); // Example address
}

void sound_init(void) {
    // Basic sound driver initialization
    // Enable PC speaker
    outb(0x61, inb(0x61) | 0x03);
}

void sound_beep(uint32_t freq, uint32_t duration_ms) {
    // Beep using PC speaker
    uint32_t div = 1193180 / freq;
    outb(0x43, 0xB6);
    outb(0x42, div & 0xFF);
    outb(0x42, (div >> 8) & 0xFF);
    // Simple delay
    for (uint32_t i = 0; i < duration_ms * 10000; i++) {
        __asm__ volatile ("nop");
    }
    // Stop beep
    outb(0x61, inb(0x61) & 0xFC);
}

void usb_init(void) {
    // Basic USB driver initialization (stub)
    // In a real implementation, this would initialize USB controllers
}