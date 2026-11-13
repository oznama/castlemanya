#pragma once

#include <psxgpu.h>
#include "../engine/cd_image.h"

#define SCREEN_XRES		320
#define SCREEN_YRES		240
#define FLOOR_LIMIT 50

void graphic_init();
void graphic_display();

void render_rect(int x, int y, int w, int h, int r, int g, int b);
void render_text(int x, int y, int z, const char *text);
void render_sprt(CD_TEXTURE tim, int x, int y, int w, int h, int u, int v, int r, int g, int b);
void render_poly_ft4(CD_TEXTURE tim, int x, int y, int w, int h, int u, int v, int r, int g, int b, uint8_t rotateu, uint8_t rotatev);