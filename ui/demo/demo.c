#include <sys/types.h>
#include <stdio.h>
#include <psxgpu.h>

#include "demo.h"
#include "../../engine/input.h"
#include "../../gameplay/many.h"
#include "../../graphics/graphic.h"
#include "../../helpers/collisions.h"

extern u_long zombie_sprites[];
TIM_IMAGE zombie_tim;

typedef struct {
	uint32_t x, y;
	uint8_t visible;
	uint32_t timer_show;
	uint32_t delay_show;
	SPRITE_OBJECT anim[1];
} ZOMBIE_OBJECT;

ZOMBIE_OBJECT zombies[4];

void zombies_init(void);
void zombies_render(MANY_OBJECT many);

void demo_game_main(void) {

	graphic_init();
	input_init();
	many_init();
	zombies_init();

	for (;;) {
		uint16_t held_buttons;
        uint16_t pressed_buttons;
        input_read(&held_buttons, &pressed_buttons);

		zombies_render(many_render(held_buttons, pressed_buttons));

		// Dibuja piso
		render_rect(
			0,
			SCREEN_YRES - FLOOR_LIMIT,
			SCREEN_XRES,
			FLOOR_LIMIT,
			0,
			255,
			0
		);


		graphic_display();
	}
}

void zombies_init(void) {
	GetTimInfo(zombie_sprites, &zombie_tim);
	printf("Rectangulo prect de zombie: (x: %d, y: %d, w: %d, h: %d)\n", 
		zombie_tim.prect->x,
		zombie_tim.prect->y,
		zombie_tim.prect->w,
		zombie_tim.prect->h
	);
	LoadImage(zombie_tim.prect, zombie_tim.paddr);
	DrawSync(0);
	if (zombie_tim.mode & 0x8)
	{
		LoadImage(zombie_tim.crect, zombie_tim.caddr);
		DrawSync(0);
	}
	for(int i=0; i<4; i++) {
		zombies[i].x = SCREEN_XRES;
		zombies[i].y = (SCREEN_YRES - 44 - FLOOR_LIMIT);
		SPRITE_OBJECT zombie_sprite  = {24, 44, 0, 0, 0, 2, 0, (!i ? 10 : 20)};
		zombies[i].anim[0] = zombie_sprite;
	}
	zombies[3].visible = 1;
	zombies[3].delay_show = 120;
	zombies[2].delay_show = 180;
	zombies[1].delay_show = 300;
	zombies[0].delay_show = 900;
}

void zombies_render(MANY_OBJECT many) {
	for(int i=0; i<4; i++) {
		if (zombies[i].visible) {
			uint16_t red = !i ? 255 : 128;
			zombies[i].x = !i ? zombies[i].x - 2 : zombies[i].x - 1;

			RectangleCollisioner many_hitbox = {
				.x = many.x,
				.y = many.y,
				.width = 64,
				.height = 64
			};
			RectangleCollisioner zombie_hitbox = {
				.x = zombies[i].x,
				.y = zombies[i].y,
				.width = 48,
				.height = 44
			};

			uint8_t hit = check_collision_AABB(many_hitbox, zombie_hitbox);

			if (hit) {
				if (many.current_state == MANY_STATE_ATTACK) {
					zombies[i].x = SCREEN_XRES;
					zombies[i].visible = 0;
				} else {
					printf("Many hurted!\n");
				}
			}

			update_animation(&zombies[i].anim[0]);
			render_sprt(
				zombie_tim,
				zombies[i].x,
				zombies[i].y,
				zombies[i].anim[0].w, zombies[i].anim[0].h,
				zombies[i].anim[0].u + (zombies[i].anim[0].current_frame * zombies[i].anim[0].w), zombies[i].anim[0].v,
				red, 128, 128);
			if (zombies[i].x <= 0) {
				zombies[i].x = SCREEN_XRES;
				zombies[i].visible = 0;
			}
		} else {
			zombies[i].timer_show++;
			if(zombies[i].timer_show == zombies[i].delay_show) {
				zombies[i].timer_show = 0;
				zombies[i].visible = 1;
			}
		}
	}
}