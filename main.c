#include <stdint.h>
#include <stdio.h>

#include "ui/menu.h"

#include "ui/gameplay.h"

int main() {

	uint8_t currentLevel = 1;
	GameState currentState = menu_display();
	
	if (currentState == GAME_STATE_CONTINUE) {
		// TODO Load data
		printf("Loading memory card\n");
		currentLevel = 6;
	}

	gameplay_main(currentLevel);

	return 0;
}