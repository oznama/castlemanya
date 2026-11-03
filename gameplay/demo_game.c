#include <psxpad.h>

#include "demo_game.h"
#include "../graphics/renderer.h"

#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define TILE_WIDTH 12
#define TILE_HEIGHT 48
#define MOVE_SPEED 2
#define FLOOR_LIMIT 50
#define FIXED_SHIFT 8
#define FIXED_ONE (1 << FIXED_SHIFT)
#define JUMP_SPEED (6 * FIXED_ONE)
#define GRAVITY (FIXED_ONE / 2)

static int x = 10;
static int32_t y_position =
	(SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT) * FIXED_ONE;
static int32_t y_velocity;
static uint8_t grounded = 1;
static uint8_t posture_active;
static int title_height = TILE_HEIGHT;

typedef struct {
	int r;
	int g;
	int b;
} SquareColor;

static SquareColor square_color = { 255, 0, 0 };

GameState demo_game_update(uint16_t held_buttons, uint16_t pressed_buttons) {
	if (pressed_buttons & (PAD_CIRCLE | PAD_START))
		return GAME_STATE_MENU;

    action_move(held_buttons);

    action_jump(pressed_buttons);

    action_change_posture(pressed_buttons);

    action_block(held_buttons);

    action_crouch(held_buttons);

    if (x < 5)
		x = 5;
	else if (x > (SCREEN_XRES - TILE_WIDTH))
		x = SCREEN_XRES - TILE_WIDTH;

	// Movimientos verticales
	// if (y < 20)
	// 	y = 20;
	// else if (y > (SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT))
	// 	y = SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT;

	return GAME_STATE_TESTS;
}

void action_block(uint16_t held_buttons)
{
	if (held_buttons & PAD_L1)
	{
		square_color.r = 255;
		square_color.g = 255;
		square_color.b = 0;
	}
	else if (posture_active)
	{
		square_color.r = 0;
		square_color.g = 0;
		square_color.b = 255;
	}
	else
	{
		square_color.r = 255;
		square_color.g = 0;
		square_color.b = 0;
	}
}

void action_change_posture(uint16_t pressed_buttons)
{
	if (pressed_buttons & PAD_UP)
		posture_active = !posture_active;
}

void action_crouch(uint16_t held_buttons)
{
    // Cambia la altura del cuadrado al presionar el botón DOWN para simular que se agacha
    title_height = (grounded && (held_buttons & PAD_DOWN))
                       ? TILE_HEIGHT / 2
                       : TILE_HEIGHT;
}

void action_jump(uint16_t pressed_buttons)
{
    // Salto
    if ((pressed_buttons & PAD_CROSS) && grounded)
    {
        y_velocity = -JUMP_SPEED;
        grounded = 0;
    }

    if (!grounded)
    {
        y_position += y_velocity;
        y_velocity += GRAVITY;

        if (y_position >=
            (SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT) * FIXED_ONE)
        {
            y_position =
                (SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT) * FIXED_ONE;
            y_velocity = 0;
            grounded = 1;
        }
    }
}

void action_move(uint16_t held_buttons)
{
    // Movimientos horizontales
    if (held_buttons & PAD_LEFT)
        x -= MOVE_SPEED;
    else if (held_buttons & PAD_RIGHT)
        x += MOVE_SPEED;
}

void demo_game_render(void) {

	// Dibuja un cuadrado en la pantalla con el color actual
	renderer_draw_rect(
		x,
		(y_position >> FIXED_SHIFT) + TILE_HEIGHT - title_height,
		TILE_WIDTH,
		title_height,
		square_color.r,
		square_color.g,
		square_color.b
	);
	renderer_draw_text(8, 8, 0, "TEST AREA");
	// Dibuja un titulo en la pantalla con las instrucciones del juego
	// renderer_draw_text(8, 32, 0, "D-PAD: MOVE");

	// Dibuja piso
	renderer_draw_rect(
		0,
		SCREEN_YRES - FLOOR_LIMIT,
		SCREEN_XRES,
		FLOOR_LIMIT,
		0,
		255,
		0
	);
}
