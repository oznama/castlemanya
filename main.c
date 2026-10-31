#include "engine/game.h"

int main(int argc, const char **argv) {
	(void) argc;
	(void) argv;

	game_init();

	for (;;)
		game_frame();

	return 0;
}
