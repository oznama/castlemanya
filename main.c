#include <stdint.h>

#include "engine/input.h"
#include "graphics/graphic.h"
#include "helpers/game_state.h"
#include "helpers/utils.h"
#include "ui/menu/main_menu.h"

int main() {

	graphic_init();
	input_init();

	uint8_t currentLevel = 1;
	GameState currentState = main_menu_display();
	
	if ( currentState == GAME_STATE_LOADING ) {
		load_level(currentLevel);
	}

	return 0;
}