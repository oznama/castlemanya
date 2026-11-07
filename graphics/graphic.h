#pragma once

#include <psxgpu.h>

void graphic_init();
void graphic_display();

void render_rect(int x, int y, int width, int height, int r, int g, int b);
void render_text(uint8_t x, uint8_t y, uint8_t z, const char *text);
void render_sprt(TIM_IMAGE tim, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);