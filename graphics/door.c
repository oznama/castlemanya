#include "../engine/cd_image.h"
#include "../graphics/graphic.h"
#include "../graphics/sprite.h"

#include "door.h"

#define DOOR_W_BASE 13
#define DOOR_H 81
#define DOOR_V 32
#define DOOR_SPRITES 10

CD_TEXTURE door_tim;

SPRITE_OBJECT door_sprites[DOOR_SPRITES] = {
    [0] = { .w = DOOR_W_BASE, .h = DOOR_H, .u = 0, .v = DOOR_V },
    [1] = { .w = 24, .h = DOOR_H, .u = 22, .v = DOOR_V },
    [2] = { .w = 33, .h = DOOR_H, .u = 54, .v = DOOR_V },
    [3] = { .w = 40, .h = DOOR_H, .u = 96, .v = DOOR_V },
    [4] = { .w = 44, .h = DOOR_H, .u = 143, .v = DOOR_V },
    [5] = { .w = 50, .h = DOOR_H, .u = 195, .v = DOOR_V },
    [6] = { .w = 55, .h = DOOR_H, .u = 0, .v = DOOR_V + DOOR_H },
    [7] = { .w = 56, .h = DOOR_H, .u = 61, .v = DOOR_V + DOOR_H },
    [8] = { .w = 55, .h = DOOR_H, .u = 122, .v = DOOR_V + DOOR_H },
    [9] = { .w = 51, .h = DOOR_H, .u = 182, .v = DOOR_V + DOOR_H }
};

void door_init(void) {
	cd_load_image("\\DOOR.TIM", &door_tim);
}

uint8_t current_frame, timer;

void door_render_in_camera(uint32_t x, uint32_t y, uint32_t cam_x, uint32_t cam_y, uint8_t rotateu, uint8_t open) {
    render_poly_ft4(
        door_tim,
        rotateu ? x - door_sprites[current_frame].w + DOOR_W_BASE - cam_x : x - cam_x, y - cam_y,
        door_sprites[current_frame].w,
        door_sprites[current_frame].h,
        door_sprites[current_frame].u, 
        door_sprites[current_frame].v,
        128, 128, 128,
        rotateu, 0);
    if (open && timer == 8 && current_frame < DOOR_SPRITES - 1) {
        current_frame++;
        timer = 0;
    } else if (!open && timer == 8 && current_frame > 0) {
        current_frame--;
        timer = 0;
    }
    timer++;
}