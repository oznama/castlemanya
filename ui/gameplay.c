#include <stdio.h>
#include <psxgpu.h>

#include "gameplay.h"
#include "../engine/cd_loader.h"
#include "../engine/game_state.h"
#include "../engine/input.h"
#include "../engine/loader.h"
#include "../gameplay/many.h"
#include "../graphics/door.h"
#include "../graphics/graphic.h"
#include "../helpers/collisions.h"
#include "../helpers/level/level_generator.h"
#include "../helpers/level/level_manager.h"

void gameplay_main(uint8_t currentLevel, uint8_t currentRoom)
{
	graphic_init();
	input_init();
	door_init();

	uint8_t spawn_at_exit = 0;
	int32_t custom_spawn_x = -1;
	int32_t custom_spawn_y = -1;

	while (currentLevel >= 1 && currentLevel <= LEVELS_COUNT)
	{
		LEVEL_WORLD level_info = LEVELS_INFO[currentLevel - 1];

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
			.hilll_tile = {.u = 48, .v = 32, .collision = TILE_COLL_SOLID},
			.hillf_tile = {.u = 32, .v = 32, .collision = TILE_COLL_SOLID},
			.hillb_tile = {.u = 32, .v = 48, .collision = TILE_COLL_SOLID},
			.hillr_tile = {.u = 48, .v = 48, .collision = TILE_COLL_SOLID},
			.stairwayl_tile = {.u = 0, .v = 32, .collision = TILE_COLL_STAIR_LEFT},
			.stairwaybl_tile = {.u = 0, .v = 48, .collision = TILE_COLL_SOLID},
			.stairwayr_tile = {.u = 16, .v = 32, .collision = TILE_COLL_STAIR_RIGHT},
			.stairwaybr_tile = {.u = 16, .v = 48, .collision = TILE_COLL_SOLID},
			.bg_tile = {.u = 48, .v = 0, .collision = TILE_COLL_NONE},
			.use_bg_tile = 1
		};

		/* Generar nivel dinamico: */
		level_generate_base(
			&current_level,
			level_info.level_data[currentRoom - 1].width_tiles,
			level_info.level_data[currentRoom - 1].height_tiles,
			level_info.level_data[currentRoom - 1].level_orientation,
			&tileset_tim,
			&theme,
			level_info.level_data[currentRoom - 1].skip_border
		);

		level_draw(
			currentLevel,
			currentRoom,
			&current_level,
			&theme
		);

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

		/* Configurar puertas de entrada y salida solo en las habitaciones de borde */
		if (level_info.enter_door.active && currentRoom == 1)
		{
			level_set_enter_door(&current_level,
			                     level_info.enter_door.x,
			                     level_info.enter_door.y,
			                     level_info.enter_door.rotateu,
			                     level_info.enter_door.open);
		}
		if (level_info.exit_door.active && currentRoom == level_info.rooms)
		{
			level_set_exit_door(&current_level,
			                    level_info.exit_door.x,
			                    level_info.exit_door.y,
			                    level_info.exit_door.rotateu,
			                    level_info.exit_door.open);
		}

		MANY_OBJECT *many_ptr = many_get_instance();
		/* Posicionar a Many segun de donde viene (inicio, nivel anterior/siguiente o hueco personalizado) */
		int32_t default_spawn_y = (current_level.pixel_height - MANY_COLLISION_HEIGHT - (3 * TILE_SIZE));
		LevelDoor *exit_door = level_get_exit_door(&current_level);
		LevelDoor *enter_door = level_get_enter_door(&current_level);

		if (custom_spawn_x >= 0 || custom_spawn_y >= 0)
		{
			int32_t spawn_x = (custom_spawn_x >= 0)
				? custom_spawn_x
				: (spawn_at_exit ? ((int32_t)current_level.pixel_width - MANY_COLLISION_WIDTH - 1) : 1);
			int32_t spawn_y = (custom_spawn_y >= 0)
				? custom_spawn_y
				: default_spawn_y;

			many_set_position(spawn_x, spawn_y);
			if (spawn_at_exit)
			{
				many_ptr->direction = 0;
			}
			else
			{
				many_ptr->direction = 1;
			}

			custom_spawn_x = -1;
			custom_spawn_y = -1;
		}
		else if (spawn_at_exit)
		{
			int32_t spawn_x = (exit_door && exit_door->active)
				? (exit_door->x - MANY_COLLISION_WIDTH - 8)
				: (int32_t)current_level.pixel_width - MANY_COLLISION_WIDTH - 1;
			many_set_position(spawn_x, default_spawn_y);
			many_ptr->direction = 0; // Mirando a la izquierda al entrar desde la salida
		}
		else
		{
			int32_t spawn_x = (enter_door && enter_door->active)
				? (enter_door->x + DOOR_W_BASE + 8)
				: 1;
			many_set_position(spawn_x, default_spawn_y);
			many_ptr->direction = 1; // Mirando a la derecha al entrar desde la entrada
		}

		uint16_t buttons_history[10] = {0};
		GameState currentState = GAME_STATE_GAME;

		DoorTransitionType transition = TRANSITION_NONE;
		int8_t transition_door_idx = -1;

		/* Bucle principal del nivel actual */
		for (;;)
		{
			uint16_t held_buttons;
			uint16_t pressed_buttons;
			input_read(&held_buttons, &pressed_buttons, buttons_history);

			/* 1. Actualizar fisicas y colisiones de Many dentro del nivel dinamico */
			many_update_in_level(held_buttons, pressed_buttons, buttons_history, &current_level);

			/*
			 * 2. Transicion por puertas en el nivel (recorriendo array de puertas)
			*/
			RectangleCollisioner many_feet_box = {
				.x = (float)many_ptr->x,
				.y = (float)((many_ptr->y >> 8) + MANY_COLLISION_HEIGHT - 16),
				.width = (float)MANY_COLLISION_WIDTH,
				.height = 16.0f
			};

			for (uint8_t d = 0; d < current_level.door_count; d++)
			{
				LevelDoor *door = &current_level.doors[d];
				if (!door->active)
					continue;

				if (door->type == DOOR_TYPE_ENTER && currentRoom == 1 && currentLevel > 1)
				{
					RectangleCollisioner enter_door_box = {
						.x = (float)(door->x - 16),
						.y = (float)(door->y + DOOR_H - 16),
						.width = (float)DOOR_W_BASE + 16.0f,
						.height = 16.0f
					};

					if (check_collision_AABB(many_feet_box, enter_door_box))
					{
						door->open = 1;
						transition = TRANSITION_ENTER_DOOR;
						transition_door_idx = (int8_t)d;
						break;
					}
				}
				else if (door->type == DOOR_TYPE_EXIT && currentRoom == level_info.rooms && currentLevel < LEVELS_COUNT)
				{
					RectangleCollisioner exit_door_box = {
						.x = (float)door->x,
						.y = (float)(door->y + DOOR_H - 16),
						.width = (float)DOOR_W_BASE + 16.0f,
						.height = 16.0f
					};

					if (check_collision_AABB(many_feet_box, exit_door_box))
					{
						door->open = 1;
						transition = TRANSITION_EXIT_DOOR;
						transition_door_idx = (int8_t)d;
						break;
					}
				}
			}

			if (transition != TRANSITION_NONE)
			{
				break;
			}

			/* Transicion por bordes del cuarto (pantallas abiertas / huecos personalizados) */
			LevelDoor *active_enter_door = level_get_enter_door(&current_level);
			LevelDoor *active_exit_door = level_get_exit_door(&current_level);
			const LevelSkipBorder *current_border = &level_info.level_data[currentRoom - 1].skip_border;

			/* Borde Izquierdo */
			if (many_ptr->x <= 0)
			{
				int32_t many_mid_y = (many_ptr->y >> 8) + (MANY_COLLISION_HEIGHT / 2);
				uint16_t many_ty = (uint16_t)(many_mid_y / TILE_SIZE);
				const BorderOpening *opening = level_border_find_opening_at(
					current_border->wall_left_openings,
					current_border->wall_left_openings_count,
					many_ty
				);

				if (opening && opening->target_room > 0)
				{
					currentRoom = opening->target_room;
					if (opening->target_level > 0)
					{
						currentLevel = opening->target_level;
					}
					spawn_at_exit = 1;
					custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : -1;
					custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : -1;
					break;
				}
				else if (currentRoom > 1)
				{
					currentRoom--;
					spawn_at_exit = 1;
					break;
				}
				else if (currentLevel > 1 && (!active_enter_door || !active_enter_door->active))
				{
					currentLevel--;
					currentRoom = LEVELS_INFO[currentLevel - 1].rooms;
					spawn_at_exit = 1;
					break;
				}
			}

			/* Borde Derecho */
			int32_t room_max_x = (int32_t)current_level.pixel_width - MANY_COLLISION_WIDTH;
			if ((int32_t)many_ptr->x >= room_max_x)
			{
				int32_t many_mid_y = (many_ptr->y >> 8) + (MANY_COLLISION_HEIGHT / 2);
				uint16_t many_ty = (uint16_t)(many_mid_y / TILE_SIZE);
				const BorderOpening *opening = level_border_find_opening_at(
					current_border->wall_right_openings,
					current_border->wall_right_openings_count,
					many_ty
				);

				if (opening && opening->target_room > 0)
				{
					currentRoom = opening->target_room;
					if (opening->target_level > 0)
					{
						currentLevel = opening->target_level;
					}
					spawn_at_exit = 0;
					custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : -1;
					custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : -1;
					break;
				}
				else if (currentRoom < level_info.rooms)
				{
					currentRoom++;
					spawn_at_exit = 0;
					break;
				}
				else if (currentLevel < LEVELS_COUNT && (!active_exit_door || !active_exit_door->active))
				{
					currentLevel++;
					currentRoom = 1;
					spawn_at_exit = 0;
					break;
				}
			}

			/* Borde Superior (Techo) */
			if ((many_ptr->y >> 8) <= 0)
			{
				int32_t many_mid_x = (int32_t)many_ptr->x + (MANY_COLLISION_WIDTH / 2);
				uint16_t many_tx = (uint16_t)(many_mid_x / TILE_SIZE);
				const BorderOpening *opening = level_border_find_opening_at(
					current_border->ceiling_openings,
					current_border->ceiling_openings_count,
					many_tx
				);

				if (opening && opening->target_room > 0)
				{
					currentRoom = opening->target_room;
					if (opening->target_level > 0)
					{
						currentLevel = opening->target_level;
					}
					custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : many_ptr->x;
					custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : -1;
					break;
				}
			}

			/* Borde Inferior (Piso) */
			int32_t room_max_y = (int32_t)current_level.pixel_height - MANY_COLLISION_HEIGHT;
			if ((many_ptr->y >> 8) >= room_max_y)
			{
				int32_t many_mid_x = (int32_t)many_ptr->x + (MANY_COLLISION_WIDTH / 2);
				uint16_t many_tx = (uint16_t)(many_mid_x / TILE_SIZE);
				const BorderOpening *opening = level_border_find_opening_at(
					current_border->floor_openings,
					current_border->floor_openings_count,
					many_tx
				);

				if (opening && opening->target_room > 0)
				{
					currentRoom = opening->target_room;
					if (opening->target_level > 0)
					{
						currentLevel = opening->target_level;
					}
					custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : many_ptr->x;
					custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : 1;
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

			sprintf(buffer, "Mundo: %d", currentLevel);
			render_text(50, 50, 0, buffer);
			sprintf(buffer, "Room: %d", currentRoom);
			render_text(50, 70, 0, buffer);

			sprintf(buffer, "X: %d", many_ptr->x);
			render_text(150, 50, 0, buffer);
			sprintf(buffer, "Y: %d", many_ptr->y >> 8);
			render_text(150, 70, 0, buffer);

			graphic_display();
		}

		/* Animacion automatica al cruzar la puerta: sin control del jugador */
		if (transition == TRANSITION_EXIT_DOOR)
		{
			LevelDoor *door = (transition_door_idx >= 0) ? level_get_door(&current_level, (uint8_t)transition_door_idx) : level_get_exit_door(&current_level);

			/* 1. Esperar a que la puerta de salida termine de abrirse */
			while (door && door->current_frame < DOOR_SPRITES - 1)
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
			int32_t target_x = door ? (door->x + 32) : ((int32_t)current_level.pixel_width);
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
				currentRoom = 1;
				spawn_at_exit = 0;
			}
		}
		else if (transition == TRANSITION_ENTER_DOOR)
		{
			LevelDoor *door = (transition_door_idx >= 0) ? level_get_door(&current_level, (uint8_t)transition_door_idx) : level_get_enter_door(&current_level);

			/* 1. Esperar a que la puerta de entrada termine de abrirse */
			while (door && door->current_frame < DOOR_SPRITES - 1)
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
			int32_t target_x = door ? (door->x - 32) : 0;
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
				currentRoom = LEVELS_INFO[currentLevel - 1].rooms;
				spawn_at_exit = 1;
			}
		}
	}
}