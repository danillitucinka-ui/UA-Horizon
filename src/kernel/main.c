#include "../include/kernel.h"
#include "../include/xp_desktop.h"
#include "../include/gui.h"
#include "../include/task_manager.h"
#include "../include/driver.h"
#include "../include/string.h"
#include "../include/memory.h"



void kernel_main(void *mb_info) {
    (void)mb_info; // Mark unused parameter

    // Basic initialization test
    terminal_init();
    printf("UA-Horizon OS v0.1.0\n");
    printf("======================\n");
    printf("Basic boot test...\n");

    fs_init();
    printf("[OK] File system\n");

    memory_init();
    printf("[OK] Memory\n");

    pit_init();
    printf("[OK] PIT timer\n");

    keyboard_init();
    printf("[OK] Keyboard\n");

    printf("Boot successful!\n");

    printf("Entering main loop. Press keys to test.\n");
    printf("Type 'help' for commands.\n");
    
    char input_buffer[256];
    int input_pos = 0;
    
    while (1) {
        keyboard_poll();
        int key = keyboard_get_char();
        
        if (key > 0) {
            if (key == '\n' || key == '\r') {
                input_buffer[input_pos] = 0;
                printf("\n> %s\n", input_buffer);
                
                if (strcmp(input_buffer, "help") == 0) {
                    printf("Commands: help, mem, clear, reboot\n");
                } else if (strcmp(input_buffer, "mem") == 0) {
                    printf("Memory: %uKB used\n", heap_allocated / 1024);
                } else if (strcmp(input_buffer, "clear") == 0) {
                    terminal_init();
                }
                
                input_pos = 0;
            } else if (key == '\b') {
                if (input_pos > 0) {
                    input_pos--;
                    printf("\b \b");
                }
            } else if (input_pos < 255) {
                input_buffer[input_pos++] = key;
                printf("%c", key);
            }
        }
        
        sleep(10);
    }
}