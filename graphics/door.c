#include "../engine/cd_loader.h"
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

void door_render_instance(
    int32_t x, int32_t y,
    int32_t cam_x, int32_t cam_y,
    uint8_t rotateu, uint8_t open,
    uint8_t *io_frame, uint8_t *io_timer)
{
    uint8_t frame_idx = io_frame ? *io_frame : current_frame;
    uint8_t timer_val = io_timer ? *io_timer : timer;

    if (frame_idx >= DOOR_SPRITES)
        frame_idx = DOOR_SPRITES - 1;

    SPRITE_OBJECT *frame = &door_sprites[frame_idx];
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
        render_poly_ft4_z(
            door_tim,
            screen_x, screen_y,
            frame->w, frame->h,
            (uint8_t)frame->u,
            (uint8_t)frame->v,
            128, 128, 128,
            rotateu, 0, 1);
    }

    if (open && timer_val >= 8 && frame_idx < DOOR_SPRITES - 1) {
        frame_idx++;
        timer_val = 0;
    } else if (!open && timer_val >= 8 && frame_idx > 0) {
        frame_idx--;
        timer_val = 0;
    }

    timer_val++;

    if (io_frame)
        *io_frame = frame_idx;
    else
        current_frame = frame_idx;

    if (io_timer)
        *io_timer = timer_val;
    else
        timer = timer_val;
}