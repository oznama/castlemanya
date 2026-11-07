#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h> // Este solo esta incluido por el free
#include <psxgpu.h>
#include <psxpad.h>

#include "demo.h"
#include "../../engine/input.h"
#include "../../graphics/graphic.h"

#define MANY_MAX_FRAMES 3

typedef enum {
	MANY_STATE_IDLE,
	MANY_STATE_RUN,
	MANY_STATE_ATTACK,
	MANY_STATE_JUMP
} ManyState;

typedef struct {
	// w: ancho del sprite
	// h: altura del sprite
	// u: posicion en coordenada x
	// v: posicion en coordenada y
	uint32_t w, h, u, v;
} SPRITE_OBJECT;

typedef struct {
	uint32_t x, y;
	uint8_t direction;
	unsigned char r, g, b;
	ManyState current_state;
	SPRITE_OBJECT amin[4][3];
} MANY_OBJECT;

MANY_OBJECT many = {
	.current_state = MANY_STATE_IDLE, // Estado base respirando
	.x = 10, // Posicion en X
	.y = 100, // Posicion en Y
	.direction = 1, // Direccion derecha
	.r = 128, // Color R
	.g = 128, // Color G
	.b = 128, // Color B
	.amin = {
		{ {64, 64, 0, 0}, {64, 64, 64, 0}, {64, 64, 128, 0} }, // sprites respirando
		{ {64, 64, 64, 0}, {64, 64, 64, 64}, {64, 64, 64, 128} }, // sprites caminando
		{ {64, 64, 128, 0}, {64, 64, 128, 64}, {64, 64, 128, 128} }, // sprites pegando
		{ {64, 64, 192, 0}, {64, 64, 192, 0}, {64, 64, 192, 0} } // sprite saltando
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

	const int16_t FIXED_SHIFT = 8;
	int16_t FIXED_ONE = (1 << FIXED_SHIFT);
	int16_t JUMP_SPEED = (8 * FIXED_ONE);
	int16_t GRAVITY = (FIXED_ONE / 2);
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
			many.current_state = MANY_STATE_RUN;
		} else if ((held_buttons & PAD_RIGHT)) {
			many.x += MOVE_SPEED;
			many.direction = 1;
			many.current_state = MANY_STATE_RUN;
		} else {
			many.current_state = MANY_STATE_IDLE;
		}

		// Many saltando
		if ((pressed_buttons & PAD_CROSS)) {
			y_velocity -= JUMP_SPEED;
			many.current_state = MANY_STATE_JUMP;
		}
		if (many.current_state = MANY_STATE_JUMP) {
			many.y += y_velocity;
			y_velocity += GRAVITY;

			if (many.y >= 100) {
				many.y = 100;
				y_velocity = 0;
				many.current_state = MANY_STATE_IDLE;
			}
		}

		render_sprt(tim,
			many.x, many.y,
			many.amin[many.current_state][many_current_frame].w, many.amin[many.current_state][many_current_frame].h,
			many.amin[many.current_state][many_current_frame].u, many.amin[many.current_state][many_current_frame].v,
			many.r, many.g, many.b
		);

		if (many_current_frame < MANY_MAX_FRAMES) {
			many_current_frame++;
		} else {
			many_current_frame = 0;
		}

		graphic_display();
	}
}