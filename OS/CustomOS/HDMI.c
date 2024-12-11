#include "hdmi.h"

static uint32_t* framebuffer;
static uint32_t current_width;
static uint32_t current_height;

void hdmi_init() {
    // Initialize framebuffer
    framebuffer = (uint32_t*)0x80000000;  // Example memory location
    current_width = HDMI_WIDTH;
    current_height = HDMI_HEIGHT;
    
    // Configure HDMI hardware
    // Set resolution and timing parameters
    // Enable display output
}

void hdmi_set_pixel(uint32_t x, uint32_t y, Color color) {
    if (x < current_width && y < current_height) {
        uint32_t pixel = (color.r << 16) | (color.g << 8) | color.b;
        framebuffer[y * current_width + x] = pixel;
    }
}

void hdmi_clear(Color color) {
    uint32_t pixel = (color.r << 16) | (color.g << 8) | color.b;
    for (uint32_t i = 0; i < current_width * current_height; i++) {
        framebuffer[i] = pixel;
    }
}

void hdmi_swap_buffers() {
    // Implement double buffering if needed
    // Update display controller
}