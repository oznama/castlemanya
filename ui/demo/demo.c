#include "demo.h"
#include "../../engine/input.h"
#include "../../gameplay/many.h"
#include "../../graphics/graphic.h"

void demo_game_main(void) {

	graphic_init();
	input_init();
	many_init();

	for (;;) {
		uint16_t held_buttons;
        uint16_t pressed_buttons;
        input_read(&held_buttons, &pressed_buttons);

		many_render(held_buttons, pressed_buttons);

		// Dibuja piso
		render_rect(
			0,
			SCREEN_YRES - FLOOR_LIMIT,
			SCREEN_XRES,
			FLOOR_LIMIT,
			0,
			255,
			0
		);


		graphic_display();
	}
}