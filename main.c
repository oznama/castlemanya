#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui/menu.h"

#include "ui/gameplay.h"

int main() {

	int dummy;
	// Usa la dirección de memoria en el stack como semilla
	srand((unsigned int)(uintptr_t)&dummy);

	uint8_t currentLevel = 1, currentRoom = 1;
	GameState currentState = menu_display();
	
	if (currentState == GAME_STATE_CONTINUE) {
		// TODO Load data
		printf("Loading memory card\n");
		currentLevel = 1;
		currentRoom = 8;
	}

	gameplay_main(currentLevel, currentRoom);

	return 0;
}