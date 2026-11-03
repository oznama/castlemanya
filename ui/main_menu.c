#include <psxpad.h>

#include "main_menu.h"
#include "../graphics/renderer.h"

static int selected_option;
static int option_count = 3;

GameState main_menu_update(uint16_t pressed_buttons) {
	if (pressed_buttons & (PAD_UP | PAD_DOWN)) {
		if (pressed_buttons & PAD_UP && selected_option > 0)
			selected_option--;
		else if (pressed_buttons & PAD_DOWN && selected_option < option_count - 1)
			selected_option++;
	}

	if (pressed_buttons & (PAD_CROSS | PAD_START)) {
		return selected_option == 0
			? GAME_STATE_GAME
			: selected_option == 1
				? GAME_STATE_SETTINGS
				: GAME_STATE_TESTS;
	}

	return GAME_STATE_MENU;
}

void main_menu_render(void) {
	int selected_y = selected_option == 0 ? 100 : selected_option == 1 ? 132 : 164;

	renderer_draw_text(60, 40, 0, "CASTLEMANYA - MAIN MENU");
	renderer_draw_rect(76, selected_y, 168, 24, 72, 36, 16);
	renderer_draw_text(92, selected_y + 4, 0, ">");
	renderer_draw_text(112, 104, 0, "START GAME");
	renderer_draw_text(112, 136, 0, "SETTINGS");
	renderer_draw_text(112, 168, 0, "TEST AREA");
	renderer_draw_text(44, 220, 0, "UP/DOWN: SELECT   CROSS: CONFIRM");
}
