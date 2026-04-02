#include "../../include/xp_desktop.h"
#include "../../include/xp_theme.h"
#include "../../include/gui.h"
#include "../../include/stdint.h"

static desktop_icon_t icons[MAX_ICONS];
static start_menu_t start_menu;
static uint32_t screen_width = 800;
static uint32_t screen_height = 600;
static int start_button_hover = 0;
static int start_button_pressed = 0;
static int taskbar_time = 0;
static char time_str[16] = "12:00";

void xp_desktop_init(void) {
    xp_theme_init();
    
    icons[0].x = 20; icons[0].y = 20;
    icons[0].icon_color = 0x00808000; 
    copy_str(icons[0].name, "My Computer");
    
    icons[1].x = 20; icons[1].y = 100;
    icons[1].icon_color = 0x00FFFF00;
    copy_str(icons[1].name, "My Documents");
    
    icons[2].x = 20; icons[2].y = 180;
    icons[2].icon_color = 0x00FF8000;
    copy_str(icons[2].name, "Recycle Bin");
    
    icons[3].x = 20; icons[3].y = 260;
    icons[3].icon_color = 0x000080FF;
    copy_str(icons[3].name, "Internet");
    
    icons[4].x = 20; icons[4].y = 340;
    icons[4].icon_color = 0x0000FF00;
    copy_str(icons[4].name, "Notepad");
    
    for (int i = 5; i < MAX_ICONS; i++) {
        icons[i].x = -1;
    }
    
    start_menu.visible = 0;
    start_menu.x = 0;
    start_menu.y = screen_height - 28 - 200;
    start_menu.width = 200;
    start_menu.height = 200;
    start_menu.selected_item = -1;
    start_menu.item_count = 7;
    copy_str(start_menu.items[0], "Programs");
    copy_str(start_menu.items[1], "Documents");
    copy_str(start_menu.items[2], "Settings");
    copy_str(start_menu.items[3], "Search");
    copy_str(start_menu.items[4], "Help and Support");
    copy_str(start_menu.items[5], "Run...");
    copy_str(start_menu.items[6], "Log Off");
}

static void copy_str(char *dest, const char *src) {
    int i = 0;
    while (src[i] && i < 31) { dest[i] = src[i]; i++; }
    dest[i] = 0;
}

void xp_desktop_draw(void) {
    gui_draw_rect(0, 0, screen_width, screen_height - 28, xp_theme.desktop_bg);
    
    for (int i = 0; i < MAX_ICONS; i++) {
        if (icons[i].x >= 0) {
            xp_render_icon(icons[i].x, icons[i].y, icons[i].name, icons[i].icon_color, icons[i].selected);
        }
    }
    
    xp_taskbar_draw();
    
    if (start_menu.visible) {
        xp_start_menu_draw();
    }
}

void xp_taskbar_draw(void) {
    int taskbar_y = screen_height - 28;
    gui_draw_rect(0, taskbar_y, screen_width, 28, xp_theme.taskbar_bg);
    
    xp_render_start_button(0, taskbar_y, 80, 28, start_button_hover, start_button_pressed);
    
    for (int i = 0; i < 3; i++) {
        int btn_x = 90 + i * 100;
        gui_draw_rect(btn_x, taskbar_y + 4, 90, 20, 0x245EDC);
        gui_draw_rect(btn_x + 2, taskbar_y + 6, 86, 16, 0x3C7CC8);
    }
    
    gui_draw_rect(screen_width - 100, taskbar_y + 2, 98, 24, 0x0A246A);
    gui_draw_text(screen_width - 90, taskbar_y + 8, time_str, 0xFFFFFF);
}

void xp_start_menu_draw(void) {
    gui_draw_rect(start_menu.x, start_menu.y, start_menu.width, start_menu.height, xp_theme.menu_bg);
    gui_draw_rect(start_menu.x, start_menu.y, start_menu.width, 2, 0x245EDC);
    gui_draw_rect(start_menu.x, start_menu.y, 2, start_menu.height, xp_theme.menu_bg);
    gui_draw_rect(start_menu.x + start_menu.width - 2, start_menu.y, 2, start_menu.height, 0x808080);
    gui_draw_rect(start_menu.x, start_menu.y + start_menu.height - 2, start_menu.width, 2, 0x808080);
    
    int divider_y = 90;
    gui_draw_rect(start_menu.x + 10, divider_y, start_menu.width - 30, 1, 0xD4D0C8);
    gui_draw_rect(start_menu.x + 10, divider_y + 1, start_menu.width - 30, 1, 0xFFFFFF);
    
    int right_panel_x = 30;
    int right_panel_y = 20;
    gui_draw_rect(right_panel_x, right_panel_y, 160, 50, 0xFFFFFF);
    gui_draw_rect(right_panel_x, right_panel_y, 160, 2, 0xD4D0C8);
    gui_draw_text(right_panel_x + 10, right_panel_y + 20, "Windows XP", 0x000080);
    gui_draw_text(right_panel_x + 10, right_panel_y + 35, "Professional", 0x808080);
    
    for (int i = 0; i < start_menu.item_count; i++) {
        int item_y = start_menu.y + 110 + i * 20;
        if (start_menu.selected_item == i) {
            gui_draw_rect(start_menu.x + 5, item_y, start_menu.width - 10, 18, xp_theme.menu_hover);
            gui_draw_text(start_menu.x + 35, item_y + 3, start_menu.items[i], 0xFFFFFF);
        } else {
            gui_draw_text(start_menu.x + 35, item_y + 3, start_menu.items[i], 0x000000);
        }
    }
    
    gui_draw_rect(start_menu.x, start_menu.y + start_menu.height - 35, start_menu.width, 2, 0xD4D0C8);
    gui_draw_rect(start_menu.x + 10, start_menu.y + start_menu.height - 30, 25, 25, xp_theme.start_button_bg);
    gui_draw_text(start_menu.x + 40, start_menu.y + start_menu.height - 22, "Shut Down", 0x000000);
}

void xp_start_menu_toggle(void) {
    start_menu.visible = !start_menu.visible;
}

void xp_render_window(int x, int y, int w, int h, const char *title, int active) {
    gui_draw_rect(x, y, w, h, xp_theme.window_bg);
    gui_draw_rect(x, y, w, 2, 0xFFFFFF);
    gui_draw_rect(x, y, 2, h, 0xFFFFFF);
    gui_draw_rect(x + w - 2, y, 2, h, 0x808080);
    gui_draw_rect(x, y + h - 2, w, 2, 0x808080);
    
    xp_render_title_bar(x, y + 2, w - 4, title, active);
}

void xp_render_title_bar(int x, int y, int w, const char *title, int active) {
    uint32_t bg = active ? xp_theme.title_bg_active : xp_theme.title_bg;
    uint32_t fg = active ? xp_theme.title_text_active : xp_theme.title_text;
    
    gui_draw_rect(x, y, w, 24, bg);
    gui_draw_text(x + 8, y + 6, title, fg);
    
    for (int i = 0; i < 3; i++) {
        int btn_x = x + w - 20 - i * 16;
        gui_draw_rect(btn_x, y + 6, 12, 14, 0xD4D0C8);
        gui_draw_rect(btn_x + 1, y + 7, 10, 12, 0xECE9D8);
    }
}

void xp_render_button(int x, int y, int w, int h, const char *text, int state) {
    uint32_t bg = xp_theme.button_bg;
    if (state == 1) bg = xp_theme.button_hover;
    if (state == 2) bg = xp_theme.button_press;
    
    gui_draw_rect(x, y, w, h, bg);
    gui_draw_rect(x, y, w, 1, 0xFFFFFF);
    gui_draw_rect(x, y, 1, h, 0xFFFFFF);
    gui_draw_rect(x + w - 1, y, 1, h, 0x808080);
    gui_draw_rect(x, y + h - 1, w, 1, 0x808080);
    
    gui_draw_text(x + (w / 2) - 20, y + (h / 2) - 6, text, 0x000000);
}

void xp_render_icon(int x, int y, const char *name, uint32_t color, int selected) {
    gui_draw_rect(x, y, 32, 32, color);
    gui_draw_rect(x + 2, y + 2, 28, 28, 0xFFFFFF);
    gui_draw_rect(x + 8, y + 8, 16, 16, color);
    
    gui_draw_rect(x + 4, y + 32, 40, 1, 0x000000);
    gui_draw_rect(x + 4, y + 33, 40, 1, 0xFFFFFF);
    
    gui_draw_text(x + 4, y + 36, name, 0xFFFFFF);
}

void xp_render_start_button(int x, int y, int w, int h, int hovered, int pressed) {
    uint32_t bg = xp_theme.start_button_bg;
    if (pressed) bg = 0x1A4C9C;
    else if (hovered) bg = 0x3C7CC8;
    
    gui_draw_rect(x, y, w, h, bg);
    
    gui_draw_text(x + 30, y + 8, "start", 0xFFFFFF);
}

void xp_taskbar_clock_update(void) {
    taskbar_time++;
    if (taskbar_time > 100) {
        taskbar_time = 0;
        int hour = (taskbar_time / 60) % 24;
        int min = taskbar_time % 60;
        time_str[0] = '0' + (hour / 10);
        time_str[1] = '0' + (hour % 10);
        time_str[3] = '0' + (min / 10);
        time_str[4] = '0' + (min % 10);
    }
}

void xp_desktop_click(int x, int y) {
    if (x >= 0 && x < 80 && y >= screen_height - 28 && y < screen_height) {
        xp_start_menu_toggle();
        return;
    }
    
    if (start_menu.visible) {
        if (x >= start_menu.x && x < start_menu.x + start_menu.width &&
            y >= start_menu.y && y < start_menu.y + start_menu.height) {
            return;
        } else {
            start_menu.visible = 0;
        }
    }
    
    for (int i = 0; i < MAX_ICONS; i++) {
        if (icons[i].x >= 0 && x >= icons[i].x && x < icons[i].x + 48 &&
            y >= icons[i].y && y < icons[i].y + 48) {
            for (int j = 0; j < MAX_ICONS; j++) {
                icons[j].selected = 0;
            }
            icons[i].selected = 1;
            break;
        }
    }
}