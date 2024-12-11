#ifndef HDMI_H
#define HDMI_H

#include <stdint.h>

// HDMI display configuration
#define HDMI_WIDTH 1920
#define HDMI_HEIGHT 1080
#define HDMI_BPP 32    // 32-bit color depth

// Color structure for 32-bit RGBA
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

void hdmi_init(void);
void hdmi_set_pixel(uint32_t x, uint32_t y, Color color);
void hdmi_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color);
void hdmi_draw_text(uint32_t x, uint32_t y, const char* text, Color color);
void hdmi_clear(Color color);
void hdmi_swap_buffers(void);

#endif
