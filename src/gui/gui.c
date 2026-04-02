#include "../../include/gui.h"
#include "../../include/stdint.h"

static gui_window_t windows[GUI_MAX_WINDOWS];
static gui_taskbar_t taskbar;
static uint32_t fb_width = 800;
static uint32_t fb_height = 600;
static uint32_t *framebuffer = 0;
static int window_count = 0;

void gui_init(void) {
    for (int i = 0; i < GUI_MAX_WINDOWS; i++) {
        windows[i].visible = 0;
    }
    taskbar.x = 0;
    taskbar.y = fb_height - 40;
    taskbar.bg_color = 0x2D2D2D;
    taskbar.text_color = 0xFFFFFF;
    taskbar.visible = 1;
}

void gui_draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (framebuffer && x < fb_width && y < fb_height) {
        framebuffer[y * fb_width + x] = color;
    }
}

void gui_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t py = y; py < y + h && py < fb_height; py++) {
        for (uint32_t px = x; px < x + w && px < fb_width; px++) {
            gui_draw_pixel(px, py, color);
        }
    }
}

void gui_draw_text(uint32_t x, uint32_t y, const char *text, uint32_t color) {
    (void)x; (void)y; (void)text; (void)color;
}

void gui_update(void) {
    gui_draw_rect(0, 0, fb_width, fb_height - 40, 0x1A1A2E);
    if (taskbar.visible) {
        gui_draw_rect(taskbar.x, taskbar.y, fb_width, 40, taskbar.bg_color);
    }
    for (int i = 0; i < GUI_MAX_WINDOWS; i++) {
        if (windows[i].visible) {
            gui_draw_rect(windows[i].x, windows[i].y, windows[i].width, windows[i].height, windows[i].bg_color);
            gui_draw_rect(windows[i].x, windows[i].y, windows[i].width, 30, 0x3D3D5C);
            gui_draw_text(windows[i].x + 10, windows[i].y + 10, windows[i].title, 0xFFFFFF);
        }
    }
}

int gui_create_window(const char *title, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    if (window_count >= GUI_MAX_WINDOWS) return -1;
    windows[window_count].x = x;
    windows[window_count].y = y;
    windows[window_count].width = w;
    windows[window_count].height = h;
    windows[window_count].bg_color = 0xFFFFFF;
    windows[window_count].border_color = 0xCCCCCC;
    windows[window_count].visible = 1;
    windows[window_count].focused = 1;
    int len = 0;
    while (title[len] && len < 63) { windows[window_count].title[len] = title[len]; len++; }
    windows[window_count].title[len] = 0;
    return window_count++;
}

void gui_close_window(int id) {
    if (id >= 0 && id < GUI_MAX_WINDOWS) {
        windows[id].visible = 0;
    }
}

void gui_set_resolution(uint32_t w, uint32_t h) {
    fb_width = w;
    fb_height = h;
}

void gui_set_framebuffer(uint32_t *fb) {
    framebuffer = fb;
}