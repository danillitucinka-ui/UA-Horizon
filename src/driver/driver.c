#include "../include/driver.h"
#include "../include/memory.h"

void memory_init(void) {
    pmm_init();
    vmm_init();
}

// PIT (Programmable Interval Timer) constants
#define PIT_FREQUENCY 1193182  // Base frequency
#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND 0x43

static uint32_t pit_ticks = 0;

void pit_init(void) {
    // Simple PIT initialization - just set a basic timer
    outb(0x43, 0x36);
    outb(0x40, 0x00);  // Low byte
    outb(0x40, 0x00);  // High byte
    pit_ticks = 0;
}

void pit_set_frequency(uint32_t frequency) {
    if (frequency == 0 || frequency > PIT_FREQUENCY) return;

    uint16_t divisor = PIT_FREQUENCY / frequency;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);
}

uint32_t pit_get_ticks(void) {
    return pit_ticks;
}

void pit_tick(void) {
    pit_ticks++;
}

#define KEYBOARD_BUFFER_SIZE 256
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64

static uint8_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int keyboard_read = 0;
static int keyboard_write = 0;
static uint8_t keyboard_leds = 0;

// US QWERTY keyboard scancode to ASCII mapping (lowercase)
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0
};

void keyboard_init(void) {
    // Simple initialization - just prepare buffers
    // Complex PS/2 initialization can cause boot issues
    keyboard_read = 0;
    keyboard_write = 0;
    keyboard_leds = 0;
}

int keyboard_get_char(void) {
    if (keyboard_read == keyboard_write) {
        return -1; // No characters available
    }

    uint8_t scancode = keyboard_buffer[keyboard_read];
    keyboard_read = (keyboard_read + 1) % KEYBOARD_BUFFER_SIZE;

    // Convert scancode to ASCII (basic implementation)
    if (scancode < sizeof(scancode_to_ascii)) {
        return scancode_to_ascii[scancode];
    }

    return 0;
}

void keyboard_poll(void) {
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);

        // Simple buffer (circular buffer)
        int next_write = (keyboard_write + 1) % KEYBOARD_BUFFER_SIZE;
        if (next_write != keyboard_read) {
            keyboard_buffer[keyboard_write] = scancode;
            keyboard_write = next_write;
        }
    }
}

void keyboard_set_leds(uint8_t leds) {
    keyboard_leds = leds;
    outb(KEYBOARD_DATA_PORT, 0xED);
    outb(KEYBOARD_DATA_PORT, leds);
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

uint32_t inl(uint16_t port) {
    uint32_t result;
    __asm__ volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t pci_read_config_simple(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)(
        (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000
    );

    outl(0xCF8, address);
    return inl(0xCFC);
}

void pci_scan(void) {
    // PCI scanning disabled during boot for stability
    printf("[PCI] PCI scanning skipped for boot stability\n");
}

void pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint32_t *data) {
    (void)bus; (void)slot; (void)func; (void)data;
}

int vbe_get_mode_info(uint16_t mode, vbe_mode_info_t *info) {
    // This would require BIOS calls in real mode
    // For now, return failure - implement when we have real mode support
    (void)mode; (void)info;
    return -1; // Not implemented
}

int vbe_set_mode(uint16_t mode) {
    // This would require BIOS calls in real mode
    // For now, return failure - implement when we have real mode support
    (void)mode;
    return -1; // Not implemented
}

void vbe_init(void) {
    // VESA graphics initialization
    // This is complex and requires real mode BIOS calls
    // For now, we'll assume text mode or a fixed framebuffer
    // In a real implementation, this would:
    // 1. Check if VBE is available
    // 2. Get available video modes
    // 3. Set a suitable graphics mode
    // 4. Set up the framebuffer

    // For now, just print that VBE is not implemented
    printf("[VBE] VESA graphics not implemented (requires real mode)\n");
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