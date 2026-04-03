#ifndef APPS_H
#define APPS_H

#include "stdint.h"

// Calculator app
int calc_main(int argc, char *argv[]);

// Text editor app
int editor_main(int argc, char *argv[]);

// Browser app (text-based)
int browser_main(int argc, char *argv[]);

// Game app (simple snake)
int game_main(int argc, char *argv[]);

// File manager app
int filemgr_main(int argc, char *argv[]);

// System monitor
int monitor_main(int argc, char *argv[]);

// Terminal app (enhanced shell)
int terminal_main(int argc, char *argv[]);

// Settings app
int settings_main(int argc, char *argv[]);

// Help app
int help_main(int argc, char *argv[]);

// Individual app functions
void apps_calculator_run(void);
void apps_file_manager_run(void);
void apps_text_editor_run(void);
void apps_snake_game_run(void);
void apps_browser_run(void);

// Drawing functions
void apps_calculator_draw(int win_id);
void apps_file_manager_draw(int win_id);
void apps_text_editor_draw(int win_id, const char *text, int cursor_pos);
void apps_snake_game_draw(int win_id, int *snake_x, int *snake_y, int length, int food_x, int food_y, int score);

// App launcher
void apps_init(void);
int apps_run(const char *app_name, int argc, char *argv[]);
void apps_list(void);

#endif