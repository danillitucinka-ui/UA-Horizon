#include "kernel.h"

void kernel_main(void *mb_info) {
    terminal_init();
    printf("UA-Horizon OS v0.1.0\n");
    printf("======================\n");
    printf("Initializing kernel...\n");
    
    memory_init();
    printf("[OK] Memory management initialized\n");
    
    pit_init();
    printf("[OK] PIT timer initialized\n");
    
    keyboard_init();
    printf("[OK] Keyboard driver loaded\n");
    
    vbe_init();
    printf("[OK] VESA graphics initialized\n");
    
    gui_init();
    printf("[OK] GUI subsystem started\n");
    
    printf("\nWelcome to UA-Horizon OS!\n");
    
    while (1) {
        keyboard_poll();
        sleep(10);
    }
}