#include <psxpad.h>
#include <stdio.h>

#include "main_menu.h"
#include "../graphics/renderer.h"
#include "../helpers/utils.h"

#define SELECTOR_COLOR_1 ((RGBColor){ 255, 0, 0 })
#define SELECTOR_COLOR_2 ((RGBColor){ 68, 12, 8 })

static int selected_option;
static int option_count = 4;

static RGBColor selector_color = SELECTOR_COLOR_1;

static float factor = 0.0f;
static float velocity = 0.02f;
static int direction = 1;

GameState main_menu_update(uint16_t pressed_buttons) {

	selector_color = interpolateColor(SELECTOR_COLOR_1, SELECTOR_COLOR_2, factor);
	factor += velocity * direction;
	if (factor >= 1.0f) {
		factor = 1.0f;
		direction = -1;
	} else if (factor <= 0.0f) {
		factor = 0.0f;
		direction = 1;
	}
	
	if (pressed_buttons & (PAD_UP | PAD_DOWN)) {
		if (pressed_buttons & PAD_UP && selected_option > 0)
			selected_option--;
		else if (pressed_buttons & PAD_DOWN && selected_option < option_count - 1)
			selected_option++;
	}

	GameState next_state = GAME_STATE_MENU;
	if (pressed_buttons & (PAD_CROSS | PAD_START)) {
		switch (selected_option)
		{
		case 0:
			next_state = GAME_STATE_GAME;
			break;
		case 1 :
			next_state = GAME_STATE_SETTINGS;
			break;
		case 2:
			next_state = GAME_STATE_TESTS;
			break;
		case 3:
			next_state = GAME_STATE_DEMO;
			break;
		default:
			return GAME_STATE_MENU;
		}
	}
	return next_state;
}

void main_menu_render(void) {
	int selected_y = 100; // Default position for the first option
	switch (selected_option) {
		case 0:
			selected_y = 95;
			break;
		case 1:
			selected_y = 115;
			break;
		case 2:
			selected_y = 135;
			break;
		case 3:
			selected_y = 155;
			break;
		default:
			selected_y = 95; // Default to the first option
			break;
	}

	renderer_draw_text(60, 40, 0, "CASTLEMANYA - MAIN MENU");

	// Selector
	renderer_draw_rect(76, selected_y, 168, 20, selector_color.r, selector_color.g, selector_color.b); // Dibuja un rectángulo detrás de la opción seleccionada
	renderer_draw_text(92, selected_y + 4, 0, ">"); // Dibuja un símbolo de flecha delante de la opción seleccionada

	renderer_draw_text(112, 100, 0, "START GAME");
	renderer_draw_text(112, 120, 0, "SETTINGS");
	renderer_draw_text(112, 140, 0, "TEST AREA");
	renderer_draw_text(112, 160, 0, "DEMO GAME");
	renderer_draw_text(25, 220, 0, "UP/DOWN: SELECT   CROSS: CONFIRM");
}
