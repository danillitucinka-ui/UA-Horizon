#include "../include/shell.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/apps.h"
#include "../include/driver.h"
#include "../include/memory.h"
#include "../include/pe_loader.h"

extern unsigned int heap_allocated;

#define SHELL_PROMPT "UA-Horizon> "
#define SHELL_MAX_CMD 256

void shell_init(void) {
    printf("UA-Horizon OS Shell\n");
    printf("Type 'help' for commands\n\n");
}

void shell_run(void) {
    char cmd[SHELL_MAX_CMD];
    
    while (1) {
        printf(SHELL_PROMPT);
        
        if (shell_read_line(cmd, SHELL_MAX_CMD) > 0) {
            shell_execute(cmd);
        }
    }
}

int shell_read_line(char *buffer, int max_len) {
    int pos = 0;
    
    while (pos < max_len - 1) {
        keyboard_poll();
        int key = keyboard_get_char();
        
        if (key > 0) {
            if (key == '\n' || key == '\r') {
                buffer[pos] = 0;
                printf("\n");
                return pos;
            } else if (key == '\b') {
                if (pos > 0) {
                    pos--;
                    printf("\b \b");
                }
            } else {
                buffer[pos++] = key;
                printf("%c", key);
            }
        }
        
        sleep(10);
    }
    
    buffer[pos] = 0;
    return pos;
}

void shell_execute(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        shell_help();
    } else if (strcmp(cmd, "calc") == 0) {
        apps_calculator_run();
    } else if (strcmp(cmd, "browser") == 0) {
        apps_browser_run();
    } else if (strcmp(cmd, "tasks") == 0) {
        // Show running tasks
        printf("Running tasks: kernel, shell\n");
    } else if (strcmp(cmd, "mem") == 0) {
        printf("Memory: %uKB used\n", heap_allocated / 1024);
    } else if (strcmp(cmd, "clear") == 0) {
        terminal_init();
    } else if (strncmp(cmd, "runpe ", 6) == 0) {
        char *filename = (char *)cmd + 6;
        printf("Loading PE file: %s\n", filename);
        pe_image_t *image = pe_load(filename);
        if (pe_is_valid(image)) {
            printf("PE image loaded, executing...\n");
            pe_execute(image);
            pe_unload(image);
        } else {
            printf("Error: %s\n", pe_get_error(image));
            if (image) pe_unload(image);
        }
    } else {
        printf("Unknown command: %s\n", cmd);
        printf("Type 'help' for available commands\n");
    }
}

void shell_help(void) {
    printf("Available commands:\n");
    printf("  help     - Show this help\n");
    printf("  calc     - Launch calculator\n");
    printf("  browser  - Launch browser\n");
    printf("  tasks    - Show running tasks\n");
    printf("  mem      - Show memory usage\n");
    printf("  clear    - Clear screen\n");
    printf("  runpe <file> - Load and execute PE file\n");
    printf("  exit     - Exit shell\n");
}