#include <stdio.h>
#include <psxgpu.h>

#include "gameplay.h"
#include "../engine/cd_loader.h"
#include "../engine/input.h"
#include "../engine/loader.h"
#include "../gameplay/many.h"
#include "../graphics/door.h"
#include "../graphics/graphic.h"
#include "../helpers/level/level_generator.h"
#include "../helpers/level/level_manager.h"

void gameplay_main(uint8_t currentLevel, uint8_t currentRoom)
{
	graphic_init();
	input_init();
	many_init();
	door_init();
	misc_init();

	uint8_t spawn_at_exit = 0;
	int32_t custom_spawn_x = -1;
	int32_t custom_spawn_y = -1;
	uint8_t previous_level = 0;

	while (currentLevel >= 1 && currentLevel <= LEVELS_COUNT)
	{
		/* Solo mostrar pantalla de carga cuando se entra a un nuevo mundo/nivel */
		if (currentLevel != previous_level)
		{
			loader_show();
			previous_level = currentLevel;
		}

		/* 1. Cargar texturas (si es nuevo nivel) y generar la habitación actual vía level_manager */
		if (level_manager_load_room(currentLevel, currentRoom) != 0)
		{
			printf("Error al cargar Room %d del Nivel %d\n", currentRoom, currentLevel);
			return;
		}

		/* 2. Ubicar a Many en la posición de spawn adecuada */
		level_manager_spawn_player(currentLevel, currentRoom, spawn_at_exit, custom_spawn_x, custom_spawn_y);
		custom_spawn_x = -1;
		custom_spawn_y = -1;

		DynamicLevel *current_level = level_manager_get_current_level();
		LEVEL_WORLD *level_info = &LEVELS_INFO[currentLevel - 1];
		MANY_OBJECT *many_ptr = many_get_instance();

		uint16_t buttons_history[10] = {0};
		char buffer[32];
		LevelTransitionResult transition = {0};

		/* Bucle principal de gameplay para el cuarto actual */
		for (;;)
		{
			uint16_t held_buttons;
			uint16_t pressed_buttons;
			input_read(&held_buttons, &pressed_buttons, buttons_history);

			/* 1. Actualizar físicas y colisiones de Many */
			many_update_in_level(held_buttons, pressed_buttons, buttons_history, current_level);

			/* 2. Evaluar transiciones (puertas o bordes/huecos) usando enteros optimizados */
			transition = level_manager_check_transitions(currentLevel, currentRoom);
			if (transition.triggered)
			{
				break;
			}

			/* 3. Actualizar y obtener cámara del nivel */
			level_update_camera(current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

			int32_t cam_x, cam_y;
			level_get_camera_pos(current_level, &cam_x, &cam_y);

			/* 4. Renderizar nivel */
			level_render(current_level);

			/* 5. Renderizar jugador Many */
			many_render_in_camera(cam_x, cam_y);

			/* 6. Renderizar entidades del nivel (misceláneos, reliquias, enemigos y combate) */
			level_manager_render_entities(currentLevel, currentRoom, cam_x, cam_y, many_ptr);

			/* 7. Información de depuración */
			sprintf(buffer, "X: %d", many_ptr->x);
			render_text(50, 20, 0, buffer);
			sprintf(buffer, "Y: %d", many_ptr->y >> 8);
			render_text(150, 20, 0, buffer);

			graphic_display();
		}

		/* Animación automática al cruzar puertas (si aplica) */
		if (transition.door_transition != TRANSITION_NONE)
		{
			level_manager_play_door_transition(&transition);
		}

		/* Aplicar destinos para el siguiente ciclo de habitación/nivel */
		currentLevel = transition.target_level;
		currentRoom = transition.target_room;
		spawn_at_exit = transition.spawn_at_exit;
		custom_spawn_x = transition.custom_spawn_x;
		custom_spawn_y = transition.custom_spawn_y;
	}
}
