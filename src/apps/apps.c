#include "../include/apps.h"
#include "../include/gui.h"
#include "../include/xp_desktop.h"
#include "../include/string.h"
#include "../include/stdio.h"

typedef struct {
    char display[32];
    double current_value;
    char operation;
    int has_value;
} calculator_state_t;

static calculator_state_t calc_state = { "0", 0.0, 0, 0 };

void apps_init(void) {
    // Initialize apps
}

int apps_run(const char *app_name, int argc, char *argv[]) {
    (void)argc; (void)argv;
    if (strcmp(app_name, "calc") == 0) {
        apps_calculator_run();
        return 0;
    }
    if (strcmp(app_name, "filemgr") == 0) {
        apps_file_manager_run();
        return 0;
    }
    if (strcmp(app_name, "editor") == 0) {
        apps_text_editor_run();
        return 0;
    }
    if (strcmp(app_name, "snake") == 0) {
        apps_snake_game_run();
        return 0;
    }
    if (strcmp(app_name, "browser") == 0) {
        apps_browser_run();
        return 0;
    }
    return -1;
}

void apps_list(void) {
    printf("Available apps:\n");
    printf("  calc - Calculator\n");
    printf("  filemgr - File Manager\n");
    printf("  editor - Text Editor\n");
    printf("  snake - Snake Game\n");
    printf("  browser - Web Browser\n");
}

void apps_calculator_run(void) {
    // Create calculator window
    int win_id = gui_create_window("Calculator", 200, 150, 250, 300);
    if (win_id < 0) return;

    // Reset calculator state
    strcpy(calc_state.display, "0");
    calc_state.current_value = 0.0;
    calc_state.operation = 0;
    calc_state.has_value = 0;

    // Main calculator loop
    while (1) {
        // Draw calculator interface
        apps_calculator_draw(win_id);

        // Handle input (simplified, would need mouse/keyboard input)
        // For now, just wait
        sleep(100);
    }
}

void apps_calculator_draw(int win_id) {
    gui_window_t *win = gui_get_window(win_id);
    if (!win) return;
    uint32_t x = win->x;
    uint32_t y = win->y;

    // Draw display
    gui_draw_rect(x + 10, y + 40, 230, 40, 0xFFFFFF);
    gui_draw_rect(x + 10, y + 40, 230, 2, 0x000000);
    gui_draw_rect(x + 10, y + 40, 2, 40, 0x000000);
    gui_draw_rect(x + 238, y + 40, 2, 40, 0x000000);
    gui_draw_rect(x + 10, y + 78, 230, 2, 0x000000);
    // Note: text drawing not implemented, so display is visual only

    // Draw buttons
    const char *buttons[5][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"},
        {"C", "", "", ""}
    };

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 4; col++) {
            if (strlen(buttons[row][col]) == 0) continue;
            uint32_t btn_x = x + 10 + col * 55;
            uint32_t btn_y = y + 90 + row * 40;
            gui_draw_rect(btn_x, btn_y, 50, 35, 0xE0E0E0);
            gui_draw_rect(btn_x, btn_y, 50, 2, 0xFFFFFF);
            gui_draw_rect(btn_x, btn_y, 2, 35, 0xFFFFFF);
            gui_draw_rect(btn_x + 48, btn_y, 2, 35, 0x808080);
            gui_draw_rect(btn_x, btn_y + 33, 50, 2, 0x808080);
            // Text not drawn
        }
    }
}

void apps_calculator_button_press(const char *button) {
    if (strcmp(button, "C") == 0) {
        strcpy(calc_state.display, "0");
        calc_state.current_value = 0.0;
        calc_state.operation = 0;
        calc_state.has_value = 0;
    } else if (strcmp(button, "=") == 0) {
        if (calc_state.has_value && calc_state.operation) {
            double result = 0.0;
            switch (calc_state.operation) {
                case '+': result = calc_state.current_value + atof(calc_state.display); break;
                case '-': result = calc_state.current_value - atof(calc_state.display); break;
                case '*': result = calc_state.current_value * atof(calc_state.display); break;
                case '/': 
                    if (atof(calc_state.display) != 0) {
                        result = calc_state.current_value / atof(calc_state.display);
                    } else {
                        strcpy(calc_state.display, "Error");
                        return;
                    }
                    break;
            }
            sprintf(calc_state.display, "%.2f", result);
            calc_state.current_value = result;
            calc_state.operation = 0;
            calc_state.has_value = 1;
        }
    } else if (strchr("+-*/", button[0])) {
        if (calc_state.has_value) {
            calc_state.operation = button[0];
        } else {
            calc_state.current_value = atof(calc_state.display);
            calc_state.operation = button[0];
            calc_state.has_value = 1;
            strcpy(calc_state.display, "0");
        }
    } else {
        // Number or dot
        if (strcmp(calc_state.display, "0") == 0) {
            strcpy(calc_state.display, button);
        } else {
            strcat(calc_state.display, button);
        }
    }
}

void apps_file_manager_run(void) {
    // Create file manager window
    int win_id = gui_create_window("File Manager", 100, 100, 500, 400);
    if (win_id < 0) return;

    // Main file manager loop
    while (1) {
        // Draw file manager interface
        apps_file_manager_draw(win_id);

        // Handle input
        sleep(100);
    }
}

void apps_file_manager_draw(int win_id) {
    gui_window_t *win = gui_get_window(win_id);
    if (!win) return;
    uint32_t x = win->x;
    uint32_t y = win->y;

    // Draw toolbar
    gui_draw_rect(x + 10, y + 40, 480, 30, 0xE0E0E0);
    gui_draw_text(x + 15, y + 45, "Address: C:\\", 0x000000);

    // Draw file list area
    gui_draw_rect(x + 10, y + 80, 480, 290, 0xFFFFFF);

    // Simulate some files
    gui_draw_text(x + 15, y + 85, "My Computer", 0x000000);
    gui_draw_text(x + 15, y + 105, "My Documents", 0x000000);
    gui_draw_text(x + 15, y + 125, "Recycle Bin", 0x000000);
    gui_draw_text(x + 15, y + 145, "Network", 0x000000);
}

void apps_text_editor_run(void) {
    // Create text editor window
    int win_id = gui_create_window("Text Editor", 150, 100, 600, 400);
    if (win_id < 0) return;

    char text_buffer[1024] = "Welcome to UA-Horizon Text Editor!\n\nType your text here...";
    int cursor_pos = strlen(text_buffer);

    // Main editor loop
    while (1) {
        // Draw editor interface
        apps_text_editor_draw(win_id, text_buffer, cursor_pos);

        // Handle input (placeholder)
        sleep(100);
    }
}

void apps_text_editor_draw(int win_id, const char *text, int cursor_pos) {
    gui_window_t *win = gui_get_window(win_id);
    if (!win) return;
    uint32_t x = win->x;
    uint32_t y = win->y;

    // Draw menu bar
    gui_draw_rect(x + 10, y + 40, 580, 30, 0xE0E0E0);
    gui_draw_text(x + 15, y + 45, "File Edit View Help", 0x000000);

    // Draw text area
    gui_draw_rect(x + 10, y + 80, 580, 310, 0xFFFFFF);
    gui_draw_rect(x + 10, y + 80, 580, 2, 0x000000);

    // Draw text
    gui_draw_text(x + 15, y + 85, text, 0x000000);

    // Draw cursor (simple)
    int cursor_x = x + 15 + (cursor_pos % 50) * 8;
    int cursor_y = y + 85 + (cursor_pos / 50) * 10;
    gui_draw_rect(cursor_x, cursor_y, 2, 10, 0x000000);
}

void apps_browser_run(void) {
    // Create browser window
    int win_id = gui_create_window("Web Browser", 100, 50, 600, 400);
    if (win_id < 0) return;

    printf("Browser app started (basic implementation)\n");
    printf("Use network functions to implement web browsing\n");

    while (1) {
        keyboard_poll();
        sleep(100);

        // Basic browser loop - in real implementation would handle URL input
        // and display web content
    }
}

void apps_snake_game_run(void) {
    // Create snake game window
    int win_id = gui_create_window("Snake Game", 250, 100, 400, 400);
    if (win_id < 0) return;

    // Initialize game state
    int snake_x[100] = {200, 190, 180};
    int snake_y[100] = {200, 200, 200};
    int snake_length = 3;
    int food_x = 300;
    int food_y = 200;
    int score = 0;
    int direction = 0; // 0=right, 1=down, 2=left, 3=up

    while (1) {
        // Handle input
        keyboard_poll();
        int key = keyboard_get_char();
        if (key == 'w' || key == 'W') direction = 3;
        else if (key == 's' || key == 'S') direction = 1;
        else if (key == 'a' || key == 'A') direction = 2;
        else if (key == 'd' || key == 'D') direction = 0;

        // Move snake
        for (int i = snake_length - 1; i > 0; i--) {
            snake_x[i] = snake_x[i-1];
            snake_y[i] = snake_y[i-1];
        }

        // Update head position
        switch (direction) {
            case 0: snake_x[0] += 10; break; // right
            case 1: snake_y[0] += 10; break; // down
            case 2: snake_x[0] -= 10; break; // left
            case 3: snake_y[0] -= 10; break; // up
        }

        // Check wall collision
        if (snake_x[0] < 0 || snake_x[0] >= 380 || snake_y[0] < 0 || snake_y[0] >= 360) {
            printf("Game Over! Score: %d\n", score);
            break;
        }

        // Check food collision
        if (snake_x[0] == food_x && snake_y[0] == food_y) {
            snake_length++;
            score += 10;
            food_x = (rand() % 38) * 10;
            food_y = (rand() % 36) * 10;
        }

        // Draw game
        apps_snake_game_draw(win_id, snake_x, snake_y, snake_length, food_x, food_y, score);

        sleep(150);
    }
}

void apps_snake_game_draw(int win_id, int *snake_x, int *snake_y, int length, int food_x, int food_y, int score) {
    gui_window_t *win = gui_get_window(win_id);
    if (!win) return;
    uint32_t x = win->x;
    uint32_t y = win->y;

    // Draw game area
    gui_draw_rect(x + 10, y + 40, 380, 350, 0x000000);

    // Draw snake
    for (int i = 0; i < length; i++) {
        gui_draw_rect(x + 10 + snake_x[i] * 19, y + 40 + snake_y[i] * 17, 18, 16, 0x00FF00);
    }

    // Draw food
    gui_draw_rect(x + 10 + food_x * 19, y + 40 + food_y * 17, 18, 16, 0xFF0000);

    // Draw score
    char score_text[32];
    sprintf(score_text, "Score: %d", score);
    gui_draw_text(x + 15, y + 45, score_text, 0xFFFFFF);
}