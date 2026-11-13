#pragma once

#include <psxgpu.h>
#include "../engine/cd_image.h"

#define SCREEN_XRES		320
#define SCREEN_YRES		240
#define FLOOR_LIMIT 50

void graphic_init();
void graphic_display();

void render_rect(int x, int y, int w, int h, int r, int g, int b);
void render_text(uint8_t x, uint8_t y, uint8_t z, const char *text);
void render_sprt(CD_TEXTURE tim, uint8_t x, uint8_t y, uint32_t w, uint32_t h, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b);
void render_poly_ft4(CD_TEXTURE tim, uint8_t x, uint8_t y, uint32_t w, uint32_t h, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b, uint8_t rotateu, uint8_t rotatev);