#pragma once

void renderer_init(void);
void renderer_present(void);
void renderer_draw_rect(int x, int y, int width, int height, int r, int g, int b);
void renderer_draw_text(int x, int y, int z, const char *text);
