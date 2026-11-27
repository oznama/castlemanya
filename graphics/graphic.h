#pragma once

#include <stdint.h>
#include <psxgpu.h>
#include "../engine/cd_loader.h"

#define SCREEN_XRES		320
#define SCREEN_YRES		240
#define GRAPHIC_FADE_STEPS 8
#define GRAPHIC_FADE_BLACK (GRAPHIC_FADE_STEPS + 1)
#define FLOOR_LIMIT 50

void graphic_init();
void graphic_display();
void graphic_fade_overlay(uint8_t level);

void render_rect(int x, int y, int w, int h, int r, int g, int b);
void render_text(int x, int y, uint8_t z, const char *text);
void render_sprt(CD_TEXTURE tim, int x, int y, uint32_t w, uint32_t h, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b);
void render_poly_ft4(CD_TEXTURE tim, int x, int y, uint32_t w, uint32_t h, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b, uint8_t rotateu, uint8_t rotatev);
void render_poly_ft4_z(CD_TEXTURE tim, int x, int y, uint32_t w, uint32_t h,
                       uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b,
                       uint8_t rotateu, uint8_t rotatev, uint32_t z);