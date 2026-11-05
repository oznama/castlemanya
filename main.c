#include <stdlib.h>
#include "engine/game.h"

int main() {

	game_init();

	for (;;)
		game_frame();

	return 0;
}