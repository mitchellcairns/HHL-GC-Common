#ifndef RGB_F_H
#define RGB_F_H

#include "adapter_includes.h"

//#define RGB_S_COLOR(r, g, b) ((rgb_s) { .color = ((r << 16) | (g << 8) | (b))})

#define COLOR_RED    (rgb_s) {.r = 0xFF, .g = 0x00, .b = 0x00}
#define COLOR_ORANGE (rgb_s) {.r = 0xFF, .g = 0x4D, .b = 0x00}
#define COLOR_YELLOW (rgb_s) {.r = 0xFF, .g = 0xE6, .b = 0x00}
#define COLOR_GREEN  (rgb_s) {.r = 0x00, .g = 0xFF, .b = 0x00}
#define COLOR_BLUE   (rgb_s) {.r = 0x00, .g = 0x00, .b = 0xFF}
#define COLOR_CYAN   (rgb_s) {.r = 0x15, .g = 0xFF, .b = 0xF1}
#define COLOR_PURPLE (rgb_s) {.r = 0x61, .g = 0x00, .b = 0xFF}
#define COLOR_PINK   (rgb_s) {.r = 0xFF, .g = 0x2B, .b = 0xD0}
#define COLOR_WHITE  (rgb_s) {.r = 0xA1, .g = 0xA1, .b = 0xA1}

void rgb_set_dirty();

void rgb_set_instant(uint32_t color);

void rgb_set_single(uint32_t color, uint8_t idx);

void rgb_set_all(uint32_t color);

void rgb_init();

void rgb_task(uint32_t timestamp);


// Defined in user space
void rgb_itf_update(rgb_s *leds);

void rgb_itf_init();

#endif
