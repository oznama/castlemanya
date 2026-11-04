#include <psxpad.h>
#include <stdio.h>

#include "demo_game.h"
#include "../engine/collisions.h"
#include "../graphics/renderer.h"

#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define TILE_WIDTH 12
#define TILE_HEIGHT 48
#define MOVE_SPEED 2
#define FLOOR_LIMIT 50
#define FIXED_SHIFT 8
#define FIXED_ONE (1 << FIXED_SHIFT)
#define JUMP_SPEED (8 * FIXED_ONE)
#define GRAVITY (FIXED_ONE / 2)
#define ATTACK_DURATION 24
#define ATTACK_PADDING 10
#define BACK_SLASH_MOVES 4

// Enemies
#define ENEMY_DELAY 100
#define ENEMY_NUMBER 4

static int x = 10;
static int32_t y_position =
	(SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT) * FIXED_ONE;
static int32_t y_velocity;
static uint8_t grounded = 1;
static uint8_t posture_active;
static uint8_t block_active;
static uint8_t slash_active;
static int x_slash_distance;
static int title_height = TILE_HEIGHT;

// Attack variables
static uint8_t attack_active = 0;
static uint8_t attack_animation_timer = ATTACK_DURATION;
static uint8_t hurted = 0;

static uint8_t enemy_delay_timer = ENEMY_DELAY;
static uint8_t enemy_inactive_index = 0;

// Score
static int score = 0;
static uint8_t life = 100;

typedef struct {
	int r;
	int g;
	int b;
} SquareColor;

typedef struct {
	int x, y; // Position
	int r, g, b; // Color
	int width, height; // Size
	int point;
	int is_active; // Active state
} GenericEnemy;

static SquareColor square_color = { 255, 0, 0 };

static GenericEnemy enemies[ENEMY_NUMBER] = {
	{ SCREEN_XRES - TILE_WIDTH, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 1 },
	{ 0, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 0 },
	{ SCREEN_XRES - TILE_WIDTH, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 0 },
	{ 0, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 0 }
};

GameState demo_game_update(uint16_t held_buttons, uint16_t pressed_buttons) {
	if (pressed_buttons & (PAD_CIRCLE | PAD_START))
		return GAME_STATE_MENU;

	if ( life > 0 ) {
		init_actions_player(held_buttons, pressed_buttons);
		if (x < 5)
			x = 5;
		else if (x > (SCREEN_XRES - TILE_WIDTH))
			x = SCREEN_XRES - TILE_WIDTH;

		// Movimientos verticales
		// if (y < 20)
		// 	y = 20;
		// else if (y > (SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT))
		// 	y = SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT;

		if (attack_active) {
			attack_animation_timer--;
			if (attack_animation_timer == 0) {
				attack_active = 0;
				attack_animation_timer = ATTACK_DURATION;
			}
		}

		if (!enemies[enemy_inactive_index].is_active) {
			enemy_delay_timer--;
			if (enemy_delay_timer == 0) {
				enemies[enemy_inactive_index].x = enemy_inactive_index % 2 == 0 ? 0 : SCREEN_XRES - TILE_WIDTH;
				enemies[enemy_inactive_index].is_active = 1;
				enemy_delay_timer = ENEMY_DELAY;
				enemy_inactive_index += 1;
				if (enemy_inactive_index >= ENEMY_NUMBER)
					enemy_inactive_index = 0;
			}
		} else {
			enemy_inactive_index += 1;
			if (enemy_inactive_index >= ENEMY_NUMBER)
				enemy_inactive_index = 0;
		}

		check_enemy_collision();

		// Enemy movement
		for (int i = 0; i < ENEMY_NUMBER; i++) {
			if (enemies[i].is_active) {
				// Calculating diff beween player and enemy
				int diff_x = x - enemies[i].x;
				// Calculate total distance between player and enemy
				int total_distance = (diff_x < 0) ? -diff_x : diff_x;

				if ( total_distance > 0 ) {
					// Move enemy towards player
					if (diff_x < 0) {
						enemies[i].x -= 1; // Move left
					} else if (diff_x > 0) {
						enemies[i].x += 1; // Move right
					}
				}
			}
		}
	}

	return GAME_STATE_TESTS;
}

void init_actions_player(uint16_t held_buttons, uint16_t pressed_buttons) {
	action_move(held_buttons);
	action_back_slash(pressed_buttons);
    action_jump(pressed_buttons);
    action_change_posture(pressed_buttons);
    action_block(held_buttons);
    action_crouch(held_buttons);
	action_attack(pressed_buttons);
}

void action_block(uint16_t held_buttons)
{
	if ((held_buttons & PAD_L1) && !attack_active)
	{
		square_color.r = 255;
		square_color.g = 255;
		square_color.b = 0;
		block_active = 1;
	}
	else if (posture_active)
	{
		square_color.r = 0;
		square_color.g = 0;
		square_color.b = 255;
		block_active = 0;
	}
	else
	{
		square_color.r = 255;
		square_color.g = 0;
		square_color.b = 0;
		block_active = 0;
	}
}

void action_change_posture(uint16_t pressed_buttons)
{
	if ((pressed_buttons & PAD_UP) && !block_active)
		posture_active = !posture_active;
}

void action_crouch(uint16_t held_buttons)
{
    // Cambia la altura del cuadrado al presionar el botón DOWN para simular que se agacha
    title_height = (grounded && (held_buttons & PAD_DOWN) && !block_active)
                       ? TILE_HEIGHT / 2
                       : TILE_HEIGHT;
}

void action_jump(uint16_t pressed_buttons)
{
    // Salto
    if (((pressed_buttons & PAD_CROSS) && grounded && !block_active) || hurted )
    {
        y_velocity = !hurted ? -JUMP_SPEED : -JUMP_SPEED / 2; // Salto más bajo si el jugador está herido
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
    if ((held_buttons & PAD_LEFT && !block_active) && !block_active)
        x -= MOVE_SPEED;
    else if ((held_buttons & PAD_RIGHT) && !block_active)
        x += MOVE_SPEED;

	// Back Slash movement
	if (slash_active || hurted) {
		x -= TILE_WIDTH / 2; // Move back by half the tile width each frame
		if ( x <= x_slash_distance ) {
			slash_active = 0;
			hurted = 0;
		}
	}
}

void action_back_slash(uint16_t pressed_buttons)
{
	if ((pressed_buttons & PAD_TRIANGLE) && !block_active) {
		slash_active = 1;
		x_slash_distance = x - (TILE_WIDTH * BACK_SLASH_MOVES); // Set the distance to move back
		if (x_slash_distance < 0) x_slash_distance = 0; // Ensure we don't go off-screen
	}
}

void action_attack(uint16_t pressed_buttons)
{
    if ((pressed_buttons & PAD_SQUARE) && !block_active)
		attack_active = 1;
}

void check_enemy_collision(void) {
	// Colision cuando el enemigo está activo y el ataque del jugador también lo está
	for (int i = 0; i < ENEMY_NUMBER; i++) {
		if (enemies[i].is_active && attack_active) {
			RectangleCollisioner player = {
				.x = x + TILE_WIDTH,
				.y = (y_position >> FIXED_SHIFT) + TILE_HEIGHT - title_height + ATTACK_PADDING,
				.width = 20,
				.height = 8
			};
			RectangleCollisioner enemy_rect = {
				.x = enemies[i].x,
				.y = enemies[i].y,
				.width = enemies[i].width,
				.height = enemies[i].height
			};
			if (check_collision_AABB(player, enemy_rect)) {
				printf("Enemy killed!\n");
				// Handle collision logic here
				enemies[i].is_active = 0; // Deactivate the enemy after collision
				score += enemies[i].point; // Increase score when enemy is killed
				printf("Score: %d\n", score); // Print the updated score
			}
		}

		// Colision cuando el enemigo está activo y el jugador no está bloqueando
		if (enemies[i].is_active && !hurted && !block_active) {
			RectangleCollisioner player = {
				.x = x,
				.y = (y_position >> FIXED_SHIFT) + TILE_HEIGHT - title_height,
				.width = TILE_WIDTH,
				.height = title_height
			};
			RectangleCollisioner enemy_rect = {
				.x = enemies[i].x,
				.y = enemies[i].y,
				.width = enemies[i].width,
				.height = enemies[i].height
			};
			if (check_collision_AABB(player, enemy_rect)) {
				hurted = 1;
				x_slash_distance = x - (TILE_WIDTH * BACK_SLASH_MOVES); // Set the distance to move back
				if (x_slash_distance < 0) x_slash_distance = 0; // Ensure we don't go off-screen
				printf("Player hurted!\n");
				// Handle collision logic here
				life -= enemies[i].point; // Decrease life when player is hurt
				printf("Life: %d\n", life); // Print the updated life
			}
		}
	}
}

void demo_game_render(void) {

	if ( life <= 0 ) {
		renderer_draw_text(100, 100, 0, "GAME OVER");
		renderer_draw_text(10, 120, 0, "Press CIRCLE or START to return to menu");
		return;
	}

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

	// Dibuja un cuadrito que representa el poder
	if (attack_active) {
		renderer_draw_rect(
			x + TILE_WIDTH,
			(y_position >> FIXED_SHIFT) + TILE_HEIGHT - title_height + ATTACK_PADDING,
			20,
			8,
			square_color.r,
			square_color.g,
			square_color.b
		);
	}

	for (int i = 0; i < ENEMY_NUMBER; i++) {
		if ( enemies[i].is_active ) {
			renderer_draw_rect(
				enemies[i].x,
				enemies[i].y,
				enemies[i].width,
				enemies[i].height,
				enemies[i].r,
				enemies[i].g,
				enemies[i].b
			);
		}
	}
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
