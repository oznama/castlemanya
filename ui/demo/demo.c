#include <stdio.h>
#include <psxgpu.h>

#include "demo.h"
#include "../../engine/cd_image.h"
#include "../../engine/input.h"
#include "../../gameplay/many.h"
#include "../../graphics/graphic.h"
#include "../../helpers/collisions.h"
#include "../../helpers/level_generator.h"

#define ZOMBIES_NUM 4
#define MEDUSAS_NUM 3

CD_TEXTURE tileset_tim;
DynamicLevel current_level;
// CD_TEXTURE zombie_tim, medusa_tim;

// typedef struct {
// 	uint32_t x, y;
// 	uint8_t visible;
// 	uint32_t timer_show;
// 	uint32_t delay_show;
// 	SPRITE_OBJECT anim[1];
// } ENEMY_OBJECT;

// ENEMY_OBJECT zombies[ZOMBIES_NUM];
// ENEMY_OBJECT medusas[MEDUSAS_NUM];

// void zombies_init(void);
// void zombies_render(MANY_OBJECT many);

// void medusas_init(void);
// void medusas_render(MANY_OBJECT many);

void demo_game_main(void) {

	graphic_init();
	input_init();
	cd_load_image("\\TILESET.TIM", &tileset_tim);
	many_init();

	/* Definicion del tema utilizando tiles de 16x16 de tileset.tim:
	 * tileset.tim es 64x64 pixeles (4x4 tiles de 16x16)
	 * (0, 0): Blue-gray masonry (Piso y paredes)
	 * (16, 0): Light stone (Techo)
	 * (0, 16): Brown rock / earth (Plataformas)
	 */
	LevelThemeTextures theme = {
		.floor_tile    = { .u = 0,  .v = 0,  .collision = TILE_COLL_SOLID },
		.wall_tile     = { .u = 0,  .v = 0,  .collision = TILE_COLL_SOLID },
		.ceiling_tile  = { .u = 16, .v = 0,  .collision = TILE_COLL_SOLID },
		.platform_tile = { .u = 0,  .v = 16, .collision = TILE_COLL_ONEWAY },
		.bg_tile       = { .u = 16, .v = 16, .collision = TILE_COLL_NONE }
	};

	/* Generar nivel dinamico de 40 x 25 tiles (640 x 400 pixeles):
	 * Mas grande que la pantalla (320x240) para scroll horizontal y vertical */
	level_generate_templated(&current_level, 40, 25, &tileset_tim, &theme);

	/* Posicionar a Many al inicio del nivel sobre el piso */
	many_set_position(64, (current_level.pixel_height - 64 - (3 * TILE_SIZE)));

	for (;;) {
		uint16_t held_buttons;
		uint16_t pressed_buttons;
		input_read(&held_buttons, &pressed_buttons);

		/* 1. Actualizar fisicas y colisiones de Many dentro del nivel dinamico */
		many_update_in_level(held_buttons, pressed_buttons, &current_level);
		MANY_OBJECT *many_ptr = many_get_instance();

		/* 2. Centrar y actualizar camara para que Many siempre este visible */
		level_update_camera(&current_level, many_ptr->x, (many_ptr->y >> 8), 64, 64);

		/* 3. Renderizar el nivel con frustum culling */
		level_render(&current_level);

		/* 4. Renderizar a Many transformado a coordenadas de camara */
		int32_t cam_x, cam_y;
		level_get_camera_pos(&current_level, &cam_x, &cam_y);
		many_render_in_camera(cam_x, cam_y);

		graphic_display();
	}
}

// void zombies_init(void) {
// 	cd_load_image("\\ZOMBIE.TIM", &zombie_tim);
// 	for(int i=0; i<ZOMBIES_NUM; i++) {
// 		zombies[i].x = SCREEN_XRES;
// 		zombies[i].y = (SCREEN_YRES - 44 - FLOOR_LIMIT);
// 		SPRITE_OBJECT zombie_sprite  = {24, 44, 0, 0, 0, 2, 0, (!i ? 10 : 20)};
// 		zombies[i].anim[0] = zombie_sprite;
// 	}
// 	zombies[3].visible = 1;
// 	zombies[3].delay_show = 120;
// 	zombies[2].delay_show = 180;
// 	zombies[1].delay_show = 300;
// 	zombies[0].delay_show = 900;
// }

// void zombies_render(MANY_OBJECT many) {
// 	for(int i=0; i<ZOMBIES_NUM; i++) {
// 		if (zombies[i].visible) {
// 			uint16_t red = !i ? 255 : 128;
// 			zombies[i].x = !i ? zombies[i].x - 2 : zombies[i].x - 1;

// 			RectangleCollisioner many_hitbox = {
// 				.x = many.x,
// 				.y = many.y,
// 				.width = 64,
// 				.height = 64
// 			};
// 			RectangleCollisioner zombie_hitbox = {
// 				.x = zombies[i].x,
// 				.y = zombies[i].y,
// 				.width = 48,
// 				.height = 44
// 			};

// 			uint8_t hit = check_collision_AABB(many_hitbox, zombie_hitbox);

// 			if (hit) {
// 				if (many.current_state == MANY_STATE_ATTACK) {
// 					zombies[i].x = SCREEN_XRES;
// 					zombies[i].visible = 0;
// 				} else {
// 					printf("Many hurted!\n");
// 				}
// 			}

// 			update_animation(&zombies[i].anim[0]);
// 			render_sprt(
// 				zombie_tim,
// 				zombies[i].x,
// 				zombies[i].y,
// 				zombies[i].anim[0].w, zombies[i].anim[0].h,
// 				zombies[i].anim[0].u + (zombies[i].anim[0].current_frame * zombies[i].anim[0].w), zombies[i].anim[0].v,
// 				red, 128, 128);
// 			if (zombies[i].x <= 0) {
// 				zombies[i].x = SCREEN_XRES;
// 				zombies[i].visible = 0;
// 			}
// 		} else {
// 			zombies[i].timer_show++;
// 			if(zombies[i].timer_show == zombies[i].delay_show) {
// 				zombies[i].timer_show = 0;
// 				zombies[i].visible = 1;
// 			}
// 		}
// 	}
// }

// void medusas_init(void) {
// 	cd_load_image("\\MEDUSA.TIM", &medusa_tim);
// 	for(int i=0; i<MEDUSAS_NUM; i++) {
// 		medusas[i].x = SCREEN_XRES;
// 		medusas[i].y = 100;
// 		SPRITE_OBJECT zombie_sprite  = {24, 24, 0, 0, 0, 3, 0, 15};
// 		medusas[i].anim[0] = zombie_sprite;
// 	}
// 	medusas[2].visible = 1;
// 	medusas[2].delay_show = 65;
// 	medusas[1].delay_show = 88;
// 	medusas[0].delay_show = 126;
// }

// void medusas_render(MANY_OBJECT many) {
// 	for(int i=0; i<MEDUSAS_NUM; i++) {
// 		if (medusas[i].visible) {
// 			medusas[i].x--;

// 			update_animation(&medusas[i].anim[0]);
// 			render_sprt(
// 				medusa_tim,
// 				medusas[i].x,
// 				medusas[i].y,
// 				medusas[i].anim[0].w, medusas[i].anim[0].h,
// 				medusas[i].anim[0].u + (medusas[i].anim[0].current_frame * medusas[i].anim[0].w), medusas[i].anim[0].v,
// 				!i ? 247 : 128, !i? 233 : 255, !i ? 30 : 128);
// 			if (medusas[i].x <= 0) {
// 				medusas[i].x = SCREEN_XRES;
// 				medusas[i].visible = 0;
// 			}
// 		} else {
// 			medusas[i].timer_show++;
// 			if(medusas[i].timer_show == medusas[i].delay_show) {
// 				medusas[i].timer_show = 0;
// 				medusas[i].visible = 1;
// 			}
// 		}
// 	}
// }