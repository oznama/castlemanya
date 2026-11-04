#include <psxpad.h>

#include "game.h"
#include "game_state.h"
#include "input.h"
#include "../gameplay/demo/demo_game.h"
#include "../gameplay/test/test_area.h"
#include "../graphics/renderer.h"
#include "../ui/main_menu.h"
#include "../ui/settings.h"

static GameState game_state;

void game_init(void) {
	renderer_init();
	input_init();
	game_state = GAME_STATE_MENU;
}

void game_frame(void) {
	uint16_t held_buttons;
	uint16_t pressed_buttons;

	input_read(&held_buttons, &pressed_buttons);

	switch (game_state) {
		case GAME_STATE_MENU:
			game_state = main_menu_update(pressed_buttons);
			break;
		case GAME_STATE_SETTINGS:
			game_state = settings_update(pressed_buttons);
			break;
		case GAME_STATE_GAME:
			// TODO
			break;
		case GAME_STATE_TESTS:
			game_state = test_area_update(held_buttons, pressed_buttons);
			break;
		case GAME_STATE_DEMO:
			game_state = demo_game_update(held_buttons, pressed_buttons);
			break;
	}

	switch (game_state) {
		case GAME_STATE_MENU:
			main_menu_render();
			break;
		case GAME_STATE_SETTINGS:
			settings_render();
			break;
		case GAME_STATE_GAME:
			// TODO
			break;
		case GAME_STATE_TESTS:
			test_area_render();
			break;
		case GAME_STATE_DEMO:
			demo_game_render();
			break;
	}

	renderer_present();
}
