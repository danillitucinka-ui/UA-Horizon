#include "../../include/kernel.h"
#include "../../include/xp_desktop.h"
#include "../../include/gui.h"
#include "../../include/task_manager.h"

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

    sound_init();
    printf("[OK] Sound driver initialized\n");

    usb_init();
    printf("[OK] USB driver initialized\n");

    // Loading screen
    printf("Loading UA-Horizon OS...\n");
    printf("[");
    for (int i = 0; i < 20; i++) {
        printf("#");
        sleep(50);
    }
    printf("] Done!\n");

    gui_init();
    printf("[OK] GUI subsystem started\n");
    
    xp_desktop_init();
    printf("[OK] XP Desktop loaded\n");
    
    task_manager_init();
    scheduler_init();
    notification_init();
    printf("[OK] Task Manager loaded\n");
    
    int t1 = task_create("DemoTask1", demo_task, PRIORITY_NORMAL, 4096);
    int t2 = task_create("DemoTask2", demo_task, PRIORITY_HIGH, 4096);
    int t3 = task_create("DemoTask3", demo_task, PRIORITY_LOW, 4096);
    
    task_start(t1);
    task_start(t2);
    task_start(t3);
    
    printf("\n=== Task Manager Demo ===\n");
    printf("Created 3 tasks with different priorities\n");
    printf("Use task_manager_print_status() to view\n");
    
    printf("\nWelcome to UA-Horizon OS!\n");
    
    while (1) {
        keyboard_poll();
        scheduler_run();
        xp_desktop_draw();
        sleep(10);
        
        if (tick_count % 100 == 0) {
            task_manager_print_status();
        }
    }
}