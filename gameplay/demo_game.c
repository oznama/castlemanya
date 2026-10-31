#include <psxpad.h>

#include "demo_game.h"
#include "../graphics/renderer.h"

#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define TILE_SIZE 64

static int x;
static int y;
static int dx = 1;
static int dy = 1;

GameState demo_game_update(uint16_t pressed_buttons) {
	if (pressed_buttons & (PAD_CIRCLE | PAD_START))
		return GAME_STATE_MENU;

	if ((x < 0) || (x > (SCREEN_XRES - TILE_SIZE)))
		dx = -dx;
	if ((y < 0) || (y > (SCREEN_YRES - TILE_SIZE)))
		dy = -dy;

	x += dx;
	y += dy;

	return GAME_STATE_GAME;
}

void demo_game_render(void) {
	renderer_draw_rect(x, y, TILE_SIZE, TILE_SIZE, 64, 64, 54);
	renderer_draw_text(8, 16, 0, "CASTLEMANYA - GAME STARTED");
	renderer_draw_text(8, 32, 0, "CIRCLE: BACK TO MENU");
}
