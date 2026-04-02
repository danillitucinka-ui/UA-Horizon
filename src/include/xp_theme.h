#ifndef XP_THEME_H
#define XP_THEME_H

#include "stdint.h"

typedef struct {
    uint32_t title_bg;
    uint32_t title_bg_active;
    uint32_t title_text;
    uint32_t title_text_active;
    uint32_t window_bg;
    uint32_t window_border;
    uint32_t button_bg;
    uint32_t button_hover;
    uint32_t button_press;
    uint32_t taskbar_bg;
    uint32_t taskbar_hover;
    uint32_t start_button_bg;
    uint32_t start_button_text;
    uint32_t desktop_bg;
    uint32_t selection;
    uint32_t scrollbar;
    uint32_t menu_bg;
    uint32_t menu_hover;
    uint32_t icon_blue;
} xp_theme_t;

extern xp_theme_t xp_theme;

void xp_theme_init(void);

#endif