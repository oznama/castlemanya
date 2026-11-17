#include <psxgpu.h>

#include "demo.h"
#include "../../engine/cd_image.h"
#include "../../engine/input.h"
#include "../../gameplay/many.h"
#include "../../graphics/graphic.h"
#include "../../helpers/level_generator.h"

#define ZOMBIES_NUM 4
#define MEDUSAS_NUM 3

CD_TEXTURE tileset_tim;
DynamicLevel current_level;

void demo_game_main(void) {

	graphic_init();
	input_init();
	cd_load_image("\\TILESET.TIM", &tileset_tim);
	many_init();

	// Texturas a utilizar en nivel
	LevelThemeTextures theme = {
		.floor_tile    = { .u = 16,  .v = 32,  .collision = TILE_COLL_SOLID },
		.wall_tile     = { .u = 16,  .v = 32,  .collision = TILE_COLL_SOLID },
		.ceiling_tile  = { .u = 16, .v = 32,  .collision = TILE_COLL_SOLID },
		.platform_tile = { .u = 0,  .v = 64, .collision = TILE_COLL_ONEWAY },
		.bg_tile       = { .u = 0, .v = 16, .collision = TILE_COLL_NONE }
	};

	/* Generar nivel dinamico de 40 x 25 tiles (640 x 400 pixeles):
	 * Mas grande que la pantalla (320x240) para scroll horizontal y vertical */
	level_generate_templated(&current_level, 40, 25, &tileset_tim, &theme);

	/* Posicionar a Many al inicio del nivel sobre el piso */
	many_set_position(64, (current_level.pixel_height - MANY_COLLISION_HEIGHT - (3 * TILE_SIZE)));

	for (;;) {
		uint16_t held_buttons;
		uint16_t pressed_buttons;
		input_read(&held_buttons, &pressed_buttons);

		/* 1. Actualizar fisicas y colisiones de Many dentro del nivel dinamico */
		many_update_in_level(held_buttons, pressed_buttons, &current_level);
		MANY_OBJECT *many_ptr = many_get_instance();

		/* 2. Centrar y actualizar camara para que Many siempre este visible */
		level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

		/* 3. Renderizar a Many transformado a coordenadas de camara */
		int32_t cam_x, cam_y;
		level_get_camera_pos(&current_level, &cam_x, &cam_y);

		many_render_in_camera(cam_x, cam_y);

		/* 4. Renderizar el nivel con frustum culling */
		level_render(&current_level);

		graphic_display();
	}
}