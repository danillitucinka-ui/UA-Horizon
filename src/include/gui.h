#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include <stdbool.h>

#define GUI_MAX_WINDOWS 16
#define GUI_MAX_BUTTONS 32

typedef struct {
    uint32_t x, y, width, height;
    uint32_t bg_color;
    uint32_t border_color;
    char title[64];
    bool visible;
    bool focused;
} gui_window_t;

typedef struct {
    uint32_t x, y, width, height;
    char text[32];
    bool clicked;
    uint32_t bg_color;
    uint32_t text_color;
} gui_button_t;

typedef struct {
    uint32_t x, y;
    uint32_t bg_color;
    uint32_t text_color;
    bool visible;
} gui_taskbar_t;

void gui_init(void);
void gui_draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void gui_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gui_draw_text(uint32_t x, uint32_t y, const char *text, uint32_t color);
void gui_update(void);
int gui_create_window(const char *title, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void gui_close_window(int id);

#endif