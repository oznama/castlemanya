#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h> // Este solo esta incluido por el free
#include <psxgpu.h>
#include <psxpad.h>

#include "demo.h"
#include "../../engine/input.h"
#include "../../graphics/graphic.h"

#define SCREEN_XRES 256
#define SCREEN_YRES 240
#define FLOOR_LIMIT 50
#define FIXED_SHIFT 8
#define FIXED_ONE (1 << FIXED_SHIFT)
#define JUMP_SPEED (8 * FIXED_ONE)
#define GRAVITY (FIXED_ONE / 2)
#define MANY_HEIGHT 64

typedef enum {
	MANY_STATE_IDLE,
	MANY_STATE_RUN,
	MANY_STATE_ATTACK,
	MANY_STATE_JUMP
} ManyState;

typedef struct {
	uint32_t w; // w: ancho del sprite
	uint32_t h; // h: altura del sprite
	uint32_t u; // u: posicion en coordenada x
	uint32_t v; // v: posicion en coordenada y
} SPRITE_OBJECT;

typedef struct {
	uint32_t x, y;
	uint8_t direction;
	uint8_t grouded;
	unsigned char r, g, b;
	ManyState current_state;
	SPRITE_OBJECT anim[10];
} MANY_OBJECT;

MANY_OBJECT many = {
	.current_state = MANY_STATE_IDLE, // Estado base respirando
	.x = 10, // Posicion en X
	.y = (SCREEN_YRES - MANY_HEIGHT - FLOOR_LIMIT) * FIXED_ONE, // Posicion en Y
	.direction = 1, // Direccion derecha
	.grouded = 1, // En el piso
	.r = 128, // Color R
	.g = 128, // Color G
	.b = 128, // Color B
	.anim = {
		{64, 64, 0, 0},
		{64, 64, 64, 0},
		{64, 64, 128, 0},
		{64, 64, 0, 64},
		{64, 64, 64, 64},
		{64, 64, 128, 64},
		{64, 64, 0, 128},
		{64, 64, 64, 128},
		{64, 64, 128, 128},
		{64, 64, 0, 192}
	}
};

// TODO Replace static load by load from CD
extern u_long many_sprites[];

void demo_game_main(void) {

	graphic_init();
	input_init();

	// Load logo image
    TIM_IMAGE tim;
    GetTimInfo(many_sprites, &tim);
    LoadImage(tim.prect, tim.paddr);
	DrawSync(0);
	if ( tim.mode & 0x8 ) {
		LoadImage(tim.crect, tim.caddr);
		DrawSync(0);
	}
    free(many_sprites);
	
	const int16_t MOVE_SPEED = 2;

	int32_t y_velocity;

	int16_t many_current_frame = 0;

	for (;;) {
		uint16_t held_buttons;
        uint16_t pressed_buttons;
        input_read(&held_buttons, &pressed_buttons);

		// Movimientos correr de many
		if ((held_buttons & PAD_LEFT )) {
			many.x -= MOVE_SPEED;
			many.direction = 0;
			if (many.grouded) {
				many.current_state = MANY_STATE_RUN;
				many_current_frame = 3;
			}
		} else if ((held_buttons & PAD_RIGHT)) {
			many.x += MOVE_SPEED;
			many.direction = 1;
			if (many.grouded) {
				many.current_state = MANY_STATE_RUN;
				many_current_frame = 3;
			}
		} else {
			if ( many.grouded ) {
				many.current_state = MANY_STATE_IDLE;
				many_current_frame = 0;
			}
		}

		// Many saltando
		if ((pressed_buttons & PAD_CROSS) && many.grouded) {
			y_velocity -= JUMP_SPEED;
			many.grouded = 0;
			many.current_state = MANY_STATE_JUMP;
			many_current_frame = 9;
		}
		if (!many.grouded) {
			many.y += y_velocity;
			y_velocity += GRAVITY;

			if (many.y >= (SCREEN_YRES - MANY_HEIGHT - FLOOR_LIMIT) * FIXED_ONE) {
				many.y = (SCREEN_YRES - MANY_HEIGHT - FLOOR_LIMIT) * FIXED_ONE;
				y_velocity = 0;
				many.grouded = 1;
			}
		}

		render_sprt(tim,
			many.x, (many.y >> FIXED_SHIFT),
			many.anim[many_current_frame].w, many.anim[many_current_frame].h,
			many.anim[many_current_frame].u, many.anim[many_current_frame].v,
			many.r, many.g, many.b
		);

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