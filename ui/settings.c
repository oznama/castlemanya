#include <psxpad.h>

#include "settings.h"
#include "../graphics/renderer.h"

GameState settings_update(uint16_t pressed_buttons) {
	if (pressed_buttons & (PAD_CIRCLE | PAD_START))
		return GAME_STATE_MENU;

	return GAME_STATE_SETTINGS;
}

void settings_render(void) {
	renderer_draw_text(112, 56, 0, "SETTINGS");
	renderer_draw_text(80, 104, 0, "D-PAD: MOVE THROUGH MENUS");
	renderer_draw_text(80, 128, 0, "CROSS: SELECT");
	renderer_draw_text(80, 152, 0, "CIRCLE: BACK");
	renderer_draw_text(72, 200, 0, "PRESS CIRCLE TO RETURN");
}
