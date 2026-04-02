#ifndef XP_DESKTOP_H
#define XP_DESKTOP_H

#include "stdint.h"

#define MAX_ICONS 12
#define MAX_MENU_ITEMS 10

typedef struct {
    char name[32];
    char path[64];
    uint32_t icon_color;
    int x, y;
    int selected;
} desktop_icon_t;

typedef struct {
    int visible;
    int x, y;
    int width, height;
    int selected_item;
    int item_count;
    char items[10][32];
} start_menu_t;

void xp_desktop_init(void);
void xp_desktop_draw(void);
void xp_desktop_click(int x, int y);
void xp_taskbar_draw(void);
void xp_start_menu_draw(void);
void xp_start_menu_toggle(void);
void xp_taskbar_clock_update(void);
void xp_render_window(int x, int y, int w, int h, const char *title, int active);
void xp_render_button(int x, int y, int w, int h, const char *text, int state);
void xp_render_title_bar(int x, int y, int w, const char *title, int active);
void xp_render_start_button(int x, int y, int w, int h, int hovered, int pressed);
void xp_render_icon(int x, int y, const char *name, uint32_t color, int selected);

#endif