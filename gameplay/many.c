#include <sys/types.h>
#include <stdlib.h> // Este solo esta incluido por el free
#include <stdio.h>
#include <psxgpu.h>
#include <psxpad.h>

#include "../graphics/graphic.h"
#include "../helpers/utils.h"

#define MOVE_SPEED 2
#define FIXED_SHIFT 8
#define FIXED_ONE (1 << FIXED_SHIFT)
#define JUMP_SPEED (8 * FIXED_ONE)
#define GRAVITY (FIXED_ONE / 2)
#define MANY_SIZE 64

typedef enum {
	MANY_STATE_IDLE,
	MANY_STATE_RUN,
	MANY_STATE_ATTACK,
	MANY_STATE_JUMP
} ManyState;

typedef struct {
	uint32_t x, y;
	uint8_t direction;
	uint8_t grouded;
	unsigned char r, g, b;
	ManyState current_state;
	SPRITE_OBJECT anim[10];
} MANY_OBJECT;

int32_t y_velocity;
uint16_t many_current_frame;

MANY_OBJECT many = {
	.current_state = MANY_STATE_IDLE, // Estado base respirando
	.x = 10, // Posicion en X
	.y = (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE, // Posicion en Y
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

void many_move_hor(uint16_t held_buttons);
void many_move_ver(uint16_t pressed_buttons);

// TODO Replace static load by load from CD
extern u_long many_sprites[];
TIM_IMAGE many_tim;

void many_init(void) {
    // Load logo image
    GetTimInfo(many_sprites, &many_tim);
    LoadImage(many_tim.prect, many_tim.paddr);
	DrawSync(0);
	if ( many_tim.mode & 0x8 ) {
		LoadImage(many_tim.crect, many_tim.caddr);
		DrawSync(0);
	}
    free(many_sprites);
}

void many_render(uint16_t held_buttons, uint16_t pressed_buttons) {
    many_move_hor(held_buttons);
    many_move_ver(pressed_buttons);

    // Rendereando a many en modo sprite simple (sin rotacion)
    // render_sprt(many_tim,
    //     many.x + 100, (many.y >> FIXED_SHIFT),
    //     many.anim[many_current_frame].w, many.anim[many_current_frame].h,
    //     many.anim[many_current_frame].u, many.anim[many_current_frame].v,
    //     many.r, many.g, many.b
    // );

    // Rendereando a many como poligono de 4 vertices (con rotacion)
    render_poly_ft4(many_tim,
        many.x, (many.y >> FIXED_SHIFT),
        many.anim[many_current_frame].w, many.anim[many_current_frame].h,
        many.anim[many_current_frame].u, many.anim[many_current_frame].v,
        many.r, many.g, many.b,
        !many.direction, 0
    );
}

void many_move_hor(uint16_t held_buttons) {
    // Movimientos correr de many
    if ((held_buttons & PAD_LEFT )) {
        if ( many.x > 0 ) many.x -= MOVE_SPEED;
        many.direction = 0;
        if (many.grouded) {
            many.current_state = MANY_STATE_RUN;
            many_current_frame = 3;
        }
    } else if ((held_buttons & PAD_RIGHT)) {
        if ( many.x < (SCREEN_XRES - (MANY_SIZE + MANY_SIZE/2)) ) many.x += MOVE_SPEED;
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
}

void many_move_ver(uint16_t pressed_buttons) {

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

        if (many.y >= (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE) {
            many.y = (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE;
            y_velocity = 0;
            many.grouded = 1;
        }
    }
}