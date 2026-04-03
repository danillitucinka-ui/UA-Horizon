#include "../include/kernel.h"
#include "../include/xp_desktop.h"
#include "../include/gui.h"
#include "../include/task_manager.h"
#include "../include/driver.h"

static void demo_task(void) {
    int tid = task_get_current()->id;
    printf("Task %d running\n", tid);
    for (int i = 0; i < 3; i++) {
        printf("  Task %d iteration %d\n", tid, i);
        task_delay(50);
    }
    task_stop(tid);
    notification_send(tid, "Task completed successfully");
}

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

    // Simple test loop
    while (1) {
        keyboard_poll();
        sleep(100);
    }
}