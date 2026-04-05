#include "../include/xp_theme.h"
#include "../include/gui.h"

xp_theme_t xp_theme = {
    0x000829BC,
    0x003B699C,
    0xFFFFFF,
    0xFFFFFF,
    0xECE9D8,
    0x808080,
    0xD4D0C8,
    0xE8E4D8,
    0xC0C0C0,
    0x245EDC,
    0x3C7CC8,
    0x245EDC,
    0xFFFFFF,
    0x3C7CC8,
    0x000080,
    0xD4D0C8,
    0xF2EFE4,
    0x000080,
    0x000080
};

void xp_theme_init(void) {
    xp_theme.title_bg = 0x000829BC;
    xp_theme.title_bg_active = 0x003B699C;
    xp_theme.title_text = 0xFFFFFF;
    xp_theme.title_text_active = 0xFFFFFF;
    xp_theme.window_bg = 0xECE9D8;
    xp_theme.window_border = 0x808080;
    xp_theme.button_bg = 0xD4D0C8;
    xp_theme.button_hover = 0xE8E4D8;
    xp_theme.button_press = 0xC0C0C0;
    xp_theme.taskbar_bg = 0x245EDC;
    xp_theme.taskbar_hover = 0x3C7CC8;
    xp_theme.start_button_bg = 0x245EDC;
    xp_theme.start_button_text = 0xFFFFFF;
    xp_theme.desktop_bg = 0x3C7CC8;
    xp_theme.selection = 0x000080;
    xp_theme.scrollbar = 0xD4D0C8;
    xp_theme.menu_bg = 0xF2EFE4;
    xp_theme.menu_hover = 0x000080;
    xp_theme.icon_blue = 0x000080;
}