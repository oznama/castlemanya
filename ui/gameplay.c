#include <stdio.h>
#include <psxgpu.h>

#include "gameplay.h"
#include "../engine/cd_image.h"
#include "../engine/game_state.h"
#include "../engine/input.h"
#include "../engine/loader.h"
#include "../gameplay/many.h"
#include "../graphics/door.h"
#include "../graphics/graphic.h"
#include "../helpers/collisions.h"
#include "../world/level_manager.h"

typedef enum
{
	TRANSITION_NONE = 0,
	TRANSITION_ENTER_DOOR,
	TRANSITION_EXIT_DOOR
} DoorTransitionType;

void gameplay_main(uint8_t currentLevel)
{
	graphic_init();
	input_init();
	door_init();

	uint8_t spawn_at_exit = 0;

	while (currentLevel >= 1 && currentLevel <= LEVELS_COUNT)
	{
		LEVEL_DATA level_info = LEVELS_INFO[currentLevel - 1];

		loader_show();
		
		CD_TEXTURE tileset_tim;
		DynamicLevel current_level;

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

		/* Generar nivel dinamico: */
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

		/* Asignar fondos al nivel */
		level_set_background(&current_level, background_tim, background_count);

		/* Configurar puertas de entrada y salida desde level_info */
		if (level_info.enter_door.active)
		{
			level_set_enter_door(&current_level,
			                     level_info.enter_door.x,
			                     level_info.enter_door.y,
			                     level_info.enter_door.rotateu,
			                     level_info.enter_door.open);
		}
		if (level_info.exit_door.active)
		{
			level_set_exit_door(&current_level,
			                    level_info.exit_door.x,
			                    level_info.exit_door.y,
			                    level_info.exit_door.rotateu,
			                    level_info.exit_door.open);
		}

		/* Posicionar a Many segun de donde viene (inicio o nivel anterior/siguiente) */
		int32_t default_spawn_y = (current_level.pixel_height - MANY_COLLISION_HEIGHT - (3 * TILE_SIZE));
		if (spawn_at_exit && current_level.exit_door.active)
		{
			many_set_position(current_level.exit_door.x - MANY_COLLISION_WIDTH - 8, default_spawn_y);
			MANY_OBJECT *m = many_get_instance();
			m->direction = 0; // Mirando a la izquierda al entrar desde la salida
		}
		else if (!spawn_at_exit && current_level.enter_door.active)
		{
			many_set_position(current_level.enter_door.x + DOOR_W_BASE + 8, default_spawn_y);
			MANY_OBJECT *m = many_get_instance();
			m->direction = 1; // Mirando a la derecha al entrar desde la entrada
		}
		else
		{
			many_set_position(64, default_spawn_y);
		}

		uint16_t buttons_history[10] = {0};
		GameState currentState = GAME_STATE_GAME;

		DoorTransitionType transition = TRANSITION_NONE;

		/* Bucle principal del nivel actual */
		for (;;)
		{
			uint16_t held_buttons;
			uint16_t pressed_buttons;
			input_read(&held_buttons, &pressed_buttons, buttons_history);

			/* 1. Actualizar fisicas y colisiones de Many dentro del nivel dinamico */
			many_update_in_level(held_buttons, pressed_buttons, buttons_history, &current_level);
			MANY_OBJECT *many_ptr = many_get_instance();

			/* 2. Deteccion de colision de Many con las puertas (solo en la base / pies) */
			RectangleCollisioner many_feet_box = {
				.x = (float)many_ptr->x,
				.y = (float)((many_ptr->y >> 8) + MANY_COLLISION_HEIGHT - 16),
				.width = (float)MANY_COLLISION_WIDTH,
				.height = 16.0f
			};

			if (current_level.exit_door.active && currentLevel < LEVELS_COUNT)
			{
				RectangleCollisioner exit_door_box = {
					.x = (float)current_level.exit_door.x,
					.y = (float)(current_level.exit_door.y + DOOR_H - 16),
					.width = (float)DOOR_W_BASE + 16.0f,
					.height = 16.0f
				};

				if (check_collision_AABB(many_feet_box, exit_door_box))
				{
					current_level.exit_door.open = 1;
					transition = TRANSITION_EXIT_DOOR;
					break;
				}
			}

			if (current_level.enter_door.active && currentLevel > 1)
			{
				RectangleCollisioner enter_door_box = {
					.x = (float)(current_level.enter_door.x - 16),
					.y = (float)(current_level.enter_door.y + DOOR_H - 16),
					.width = (float)DOOR_W_BASE + 16.0f,
					.height = 16.0f
				};

				if (check_collision_AABB(many_feet_box, enter_door_box))
				{
					current_level.enter_door.open = 1;
					transition = TRANSITION_ENTER_DOOR;
					break;
				}
			}

			/* 3. Centrar y actualizar camara */
			level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

			/* 4. Obtener posicion de camara */
			int32_t cam_x, cam_y;
			level_get_camera_pos(&current_level, &cam_x, &cam_y);

			/* 5. Renderizar nivel (incluye puertas) */
			level_render(&current_level);

			/* 6. Renderizar Many encima del nivel */
			many_render_in_camera(cam_x, cam_y);

			graphic_display();
		}

		/* Animacion automatica al cruzar la puerta: sin control del jugador */
		if (transition == TRANSITION_EXIT_DOOR)
		{
			/* 1. Esperar a que la puerta de salida termine de abrirse */
			while (current_level.exit_door.current_frame < DOOR_SPRITES - 1)
			{
				MANY_OBJECT *many_ptr = many_get_instance();
				level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

				int32_t cam_x, cam_y;
				level_get_camera_pos(&current_level, &cam_x, &cam_y);

				level_render(&current_level);
				many_render_in_camera(cam_x, cam_y);
				graphic_display();
			}

			/* 2. Many corre hacia y cruza la puerta hacia la derecha */
			int32_t target_x = current_level.exit_door.x + 32;
			MANY_OBJECT *many_ptr = many_get_instance();
			while ((int32_t)many_ptr->x < target_x)
			{
				many_auto_walk_step(1);

				level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

				int32_t cam_x, cam_y;
				level_get_camera_pos(&current_level, &cam_x, &cam_y);

				level_render(&current_level);
				many_render_in_camera(cam_x, cam_y);
				graphic_display();
			}

			if (currentLevel < LEVELS_COUNT)
			{
				currentLevel++;
				spawn_at_exit = 0;
			}
		}
		else if (transition == TRANSITION_ENTER_DOOR)
		{
			/* 1. Esperar a que la puerta de entrada termine de abrirse */
			while (current_level.enter_door.current_frame < DOOR_SPRITES - 1)
			{
				MANY_OBJECT *many_ptr = many_get_instance();
				level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

				int32_t cam_x, cam_y;
				level_get_camera_pos(&current_level, &cam_x, &cam_y);

				level_render(&current_level);
				many_render_in_camera(cam_x, cam_y);
				graphic_display();
			}

			/* 2. Many corre hacia y cruza la puerta hacia la izquierda */
			int32_t target_x = current_level.enter_door.x - 32;
			MANY_OBJECT *many_ptr = many_get_instance();
			while ((int32_t)many_ptr->x > target_x)
			{
				many_auto_walk_step(-1);

				level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

				int32_t cam_x, cam_y;
				level_get_camera_pos(&current_level, &cam_x, &cam_y);

				level_render(&current_level);
				many_render_in_camera(cam_x, cam_y);
				graphic_display();
			}

			if (currentLevel > 1)
			{
				currentLevel--;
				spawn_at_exit = 1;
			}
		}
	}
}