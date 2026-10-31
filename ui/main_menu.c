#include <psxpad.h>

#include "main_menu.h"
#include "../graphics/renderer.h"

static int selected_option;

GameState main_menu_update(uint16_t pressed_buttons) {
	if (pressed_buttons & (PAD_UP | PAD_DOWN))
		selected_option ^= 1;

	if (pressed_buttons & (PAD_CROSS | PAD_START)) {
		return selected_option == 0
			? GAME_STATE_GAME
			: GAME_STATE_SETTINGS;
	}

	return GAME_STATE_MENU;
}

void main_menu_render(void) {
	int selected_y = selected_option == 0 ? 100 : 132;

	renderer_draw_text(112, 56, 0, "CASTLEMANYA");
	renderer_draw_text(112, 72, 0, "MAIN MENU");
	renderer_draw_rect(76, selected_y, 168, 24, 72, 36, 16);
	renderer_draw_text(92, selected_y + 4, 0, ">");
	renderer_draw_text(112, 104, 0, "START GAME");
	renderer_draw_text(112, 136, 0, "SETTINGS");
	renderer_draw_text(44, 200, 0, "UP/DOWN: SELECT   CROSS: CONFIRM");
}
