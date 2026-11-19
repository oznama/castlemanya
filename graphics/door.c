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

void door_render_in_camera(
    int32_t x, int32_t y,
    int32_t cam_x, int32_t cam_y,
    uint8_t rotateu, uint8_t open)
{
    SPRITE_OBJECT *frame = &door_sprites[current_frame];
    int32_t frame_w = (int32_t)frame->w;
    int32_t frame_h = (int32_t)frame->h;

    int32_t screen_x = rotateu
        ? x - frame_w + DOOR_W_BASE - cam_x
        : x - cam_x;
    int32_t screen_y = y - cam_y;

    if (screen_x < SCREEN_XRES &&
        screen_x + frame_w > 0 &&
        screen_y < SCREEN_YRES &&
        screen_y + frame_h > 0)
    {
        render_poly_ft4(
            door_tim,
            screen_x, screen_y,
            frame->w, frame->h,
            (uint8_t)frame->u,
            (uint8_t)frame->v,
            128, 128, 128,
            rotateu, 0);
    }

    if (open && timer == 8 && current_frame < DOOR_SPRITES - 1) {
        current_frame++;
        timer = 0;
    } else if (!open && timer == 8 && current_frame > 0) {
        current_frame--;
        timer = 0;
    }

    timer++;
}