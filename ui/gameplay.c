#include <stdio.h>
#include <psxgpu.h>

#include "gameplay.h"
#include "../engine/cd_image.h"
#include "../engine/game_state.h"
#include "../engine/input.h"
#include "../gameplay/many.h"
#include "../graphics/graphic.h"
#include "../world/level_manager.h"

void gameplay_main(uint8_t currentLevel)
{

	LEVEL_DATA level_info = LEVELS_INFO[currentLevel - 1];
	GameState currentState = GAME_STATE_LOADING;

	CD_TEXTURE tileset_tim;
	DynamicLevel current_level;

	graphic_init();
	input_init();
	char buffer[30];
	sprintf(buffer, "\\%s.TIM", level_info.tilename);
	cd_load_image(buffer, &tileset_tim);
	many_init();

	// Texturas a utilizar en nivel
	LevelThemeTextures theme = {
		.floor_tile = {.u = 0, .v = 0, .collision = TILE_COLL_SOLID},
		.filled_tile = {.u = 0, .v = 16, .collision = TILE_COLL_SOLID},
		.wall_tile = {.u = 32, .v = 0, .collision = TILE_COLL_SOLID},
		.ceiling_tile = {.u = 16, .v = 0, .collision = TILE_COLL_SOLID},
		.pltfrml_tile = {.u = 16, .v = 16, .collision = TILE_COLL_ONEWAY},
		.pltfrmf_tile = {.u = 32, .v = 16, .collision = TILE_COLL_ONEWAY},
		.pltfrmr_tile = {.u = 48, .v = 16, .collision = TILE_COLL_ONEWAY},
		.bg_tile = {.u = 48, .v = 0, .collision = TILE_COLL_NONE}};

	/* Generar nivel dinamico de 40 x 25 tiles (640 x 400 pixeles):
	 * Mas grande que la pantalla (320x240) para scroll horizontal y vertical */
	level_generate_base(&current_level, level_info.width_tiles, level_info.height_tiles, &tileset_tim, &theme);

	level_draw(currentLevel, &current_level, &theme);

	uint8_t background_count = 0;
	CD_TEXTURE background_tim[BACKGROUND_MAX] = {};

	for (int i = 0; i < BACKGROUND_MAX; i++)
	{
		if (level_info.bgname[i][0] == '\0')
			continue;

		sprintf(buffer, "\\%s.TIM", level_info.bgname[i]);

		int result = cd_load_image(
			buffer,
			&background_tim[background_count]);

		if (result != 0)
		{
			printf("Error al cargar %s (codigo %d)\n", buffer, result);
			return;
		}

		background_count++;
	}

	/* Después de level_generate_templated(...) */
	level_set_background(&current_level, background_tim, background_count);

	/* Posicionar a Many al inicio del nivel sobre el piso */
	many_set_position(64, (current_level.pixel_height - MANY_COLLISION_HEIGHT - (3 * TILE_SIZE)));

	uint16_t buttons_history[10] = {0};

	currentState = GAME_STATE_GAME;
	for (;;)
	{
		uint16_t held_buttons;
		uint16_t pressed_buttons;
		input_read(&held_buttons, &pressed_buttons, buttons_history);

		/* 1. Actualizar fisicas y colisiones de Many dentro del nivel dinamico */
		many_update_in_level(held_buttons, pressed_buttons, buttons_history, &current_level);
		MANY_OBJECT *many_ptr = many_get_instance();

		/* 2. Centrar y actualizar camara para que Many siempre este visible */
		level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

		/* 3. Obtener posición de cámara */
		int32_t cam_x, cam_y;
		level_get_camera_pos(&current_level, &cam_x, &cam_y);

		/* 4. Renderizar primero el nivel */
		level_render(&current_level);

		/* 5. Renderizar Many encima del nivel */
		many_render_in_camera(cam_x, cam_y);

		graphic_display();
	}
}