#pragma once

#include <stdint.h>

typedef struct {
	int r;
	int g;
	int b;
} SquareColor;

typedef struct {
	int x, y; // Position
	int r, g, b; // Color
	int width, height; // Size
	int point;
	int is_active; // Active state
} GenericEnemy;

void demo_game_update(uint16_t held_buttons, uint16_t pressed_buttons);
void demo_game_render(void);