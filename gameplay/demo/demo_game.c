#include <psxpad.h>
#include <stdio.h>

#include "demo_game.h"
#include "../../graphics/renderer.h"
#include "../../helpers/collisions.h"

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
#define ATTACK_WIDTH_BASE 10
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
static uint8_t direction;
static int x_slash_distance;
static int title_height = TILE_HEIGHT;

// Attack variables
static uint8_t attack_x_position;
static uint8_t attack_active = 0;
static uint8_t attack_animation_timer = ATTACK_DURATION;
static uint8_t attack_width = ATTACK_WIDTH_BASE;
static uint8_t hurted = 0;

static uint8_t enemy_delay_timer = ENEMY_DELAY;
static uint8_t enemy_inactive_index = 0;

// Score
static int score = 0;
static uint8_t life = 100;
static uint8_t level = 1;

static SquareColor square_color = { 255, 0, 0 };

static GenericEnemy enemies[ENEMY_NUMBER] = {
	{ SCREEN_XRES - TILE_WIDTH, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 1 },
	{ 0, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 0 },
	{ SCREEN_XRES - TILE_WIDTH, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 0 },
	{ 0, SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT, 205, 205, 205, TILE_WIDTH, TILE_HEIGHT, 10, 0 }
};

GameState demo_game_update(uint16_t held_buttons, uint16_t pressed_buttons) {
	if (pressed_buttons & PAD_SELECT) {
		reset_demo_game(pressed_buttons, 1);
		return GAME_STATE_MENU;
	}

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

		attack_x_position = direction ? x - attack_width : x + TILE_WIDTH;

		if (!enemies[enemy_inactive_index].is_active) {
			enemy_delay_timer--;
			if (enemy_delay_timer == 0) {
				enemies[enemy_inactive_index].x = enemy_inactive_index % 2 == 0 ? 0 : SCREEN_XRES - TILE_WIDTH;
				enemies[enemy_inactive_index].is_active = 1;
				enemy_delay_timer = ENEMY_DELAY - (level * 10);
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
						enemies[i].x -= level; // Move left
					} else if (diff_x > 0) {
						enemies[i].x += level; // Move right
					}
				}
			}
		}
	} else {
		// Reset game state when life is 0
		reset_demo_game(pressed_buttons, 0);
	}

	return GAME_STATE_DEMO;
}

void demo_game_render(void) {

	if ( life <= 0 ) {
		renderer_draw_text(100, 80, 0, "GAME OVER");
		renderer_draw_text(36, 160, 0, "Press START to restart the game");
		renderer_draw_text(36, 180, 0, "Press SELECT to return to menu");
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

	// Dibuja un cuadrito que representa el arma
	if (attack_active) {
		renderer_draw_rect(
			attack_x_position,
			(y_position >> FIXED_SHIFT) + TILE_HEIGHT - title_height + ATTACK_PADDING,
			attack_width,
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

	// Draw life
	char life_text[20];
	sprintf(life_text, "LIFE: %d", life);
	renderer_draw_text(5, 8, 0, life_text);

	renderer_draw_text(SCREEN_XRES - 100, 8, 0, "DEMO GAME"); // Draw test area label

	// Draw level
	char level_text[20];
	sprintf(level_text, "LEVEL: %d", level);
	renderer_draw_text(5, 20, 0, level_text);

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

void reset_demo_game(uint16_t pressed_buttons, uint8_t reset_game_state) {
	if ((pressed_buttons & PAD_START) || reset_game_state) {
			life = 100;
			score = 0;
			level = 1;
			x = 10;
			y_position = (SCREEN_YRES - TILE_HEIGHT - FLOOR_LIMIT) * FIXED_ONE;
			y_velocity = 0;
			grounded = 1;
			direction = 0;
			posture_active = 0;
			block_active = 0;
			slash_active = 0;
			x_slash_distance = 0;
			title_height = TILE_HEIGHT;
			attack_active = 0;
			attack_width = ATTACK_WIDTH_BASE;
			attack_animation_timer = ATTACK_DURATION;
			hurted = 0;

			enemy_delay_timer = ENEMY_DELAY;
			enemy_inactive_index = 0;

			for (int i = 0; i < ENEMY_NUMBER; i++) {
				enemies[i].is_active = (i == 0) ? 1 : 0; // Only the first enemy is active at the start
				enemies[i].x = (i % 2 == 0) ? SCREEN_XRES - TILE_WIDTH : 0; // Position enemies on opposite sides
				enemies[i].y = SCREEN_YRES - FLOOR_LIMIT - TILE_HEIGHT; // Position enemies on the floor
				enemies[i].r = 205; // Set enemy color to gray
				enemies[i].g = 205;
				enemies[i].b = 205;
				enemies[i].width = TILE_WIDTH; // Set enemy size
				enemies[i].height = TILE_HEIGHT;
				enemies[i].point = 10; // Set points for each enemy
			}
		}
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
    if ((held_buttons & PAD_LEFT && !block_active) && !block_active && !attack_active) {
        x -= MOVE_SPEED;
		direction = 1;
	} else if ((held_buttons & PAD_RIGHT) && !block_active && !attack_active) {
        x += MOVE_SPEED;
		direction = 0;
	}

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
			RectangleCollisioner player_attack = {
				.x = attack_x_position,
				.y = (y_position >> FIXED_SHIFT) + TILE_HEIGHT - title_height + ATTACK_PADDING,
				.width = attack_width,
				.height = 8
			};
			RectangleCollisioner enemy_rect = {
				.x = enemies[i].x,
				.y = enemies[i].y,
				.width = enemies[i].width,
				.height = enemies[i].height
			};
			if (check_collision_AABB(player_attack, enemy_rect)) {
				// Handle collision logic here
				enemies[i].is_active = 0; // Deactivate the enemy after collision
				score += enemies[i].point; // Increase score when enemy is killed
				if ( score % 100 == 0 && level < 10 ) {
					level += 1;
					if ( level <= 3 ) {
						attack_width += (ATTACK_WIDTH_BASE * level);
						printf("Attack powered up!\n");
					}
				}
				printf("Enemy killed, score: %d, attack_width: %d\n", score, attack_width);
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
				// Handle collision logic here
				life -= enemies[i].point; // Decrease life when player is hurt
			}
		}
	}
}