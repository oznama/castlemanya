#include <stdio.h>
#include <psxgpu.h>

#include "demo.h"
#include "../../engine/cd_image.h"
#include "../../engine/input.h"
#include "../../gameplay/many.h"
#include "../../graphics/graphic.h"
#include "../../helpers/collisions.h"
#include "../../ui/level01/level01.h"

#define ZOMBIES_NUM 4
#define MEDUSAS_NUM 3

CD_TEXTURE level1_tim;
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

void load_level(void);
void render_level();

// void zombies_init(void);
// void zombies_render(MANY_OBJECT many);

// void medusas_init(void);
// void medusas_render(MANY_OBJECT many);

void demo_game_main(void) {

	graphic_init();
	input_init();
	load_level();
	many_init();
	// zombies_init();
	// medusas_init();

	for (;;) {
		uint16_t held_buttons;
        uint16_t pressed_buttons;
        input_read(&held_buttons, &pressed_buttons);

		MANY_OBJECT many = many_render(held_buttons, pressed_buttons, level_1_map);
		// zombies_render(many);
		// medusas_render(many);

		render_level();

		graphic_display();
	}
}

void load_level(void) {
	cd_load_image("\\LEVEL1.TIM", &level1_tim);
}

void render_level() {
	CD_TEXTURE right_part = level1_tim;
    right_part.image_x += 64;

    render_sprt(level1_tim, 0,   0, 256, 240, 0, 0, 128, 128, 128);
    render_sprt(right_part, 256, 0,  64, 240, 0, 0, 128, 128, 128);
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