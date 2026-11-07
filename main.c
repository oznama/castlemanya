#include <stdint.h>

#include "helpers/game_state.h"
#include "ui/menu/main_menu.h"

#include "ui/demo/demo.h"

int main() {

	uint8_t currentLevel = 1;
	GameState currentState = main_menu_display();
	
	if ( currentState == GAME_STATE_LOADING ) {
		// load_level(currentLevel);
		demo_game_main();
	}

	return 0;
}