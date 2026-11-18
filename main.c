#include <stdint.h>

#include "ui/menu/main_menu.h"

#include "ui/demo/demo.h"

int main() {

	// uint8_t currentLevel = 1;
	GameState currentState = main_menu_display();
	
	demo_game_main(currentState);

	return 0;
}