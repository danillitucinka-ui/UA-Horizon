#include "../include/kernel.h"
#include "../include/xp_desktop.h"
#include "../include/gui.h"
#include "../include/task_manager.h"
#include "../include/driver.h"
#include "../include/pe_loader.h"
#include "../include/string.h"
#include "../include/memory.h"
#include "../include/shell.h"



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

    shell_init();
    shell_run();
}