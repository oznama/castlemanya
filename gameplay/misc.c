#include "misc.h"

#include <psxgpu.h>

#include "../engine/cd_loader.h"
#include "../graphics/graphic.h"
#include "../graphics/sprite.h"
#include "../helpers/collisions.h"

#define RELICS_SPRITES 6
#define MISC_SPRITES 2

CD_TEXTURE relics_tim;

void misc_init(void)
{
    cd_load_image("\\MANYREL.TIM", &relics_tim);
}

uint8_t relic_render_in_camera(MiscObject *relicObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height)
{
    SPRITE_OBJECT relic_sprites[RELICS_SPRITES] = {
        // El 0 se omite
        [RELIC_VAMPIRE_KILLER] = { .u = 0, .v = 128 },
        [RELIC_DOBLE_JUMP] = { .u = 16, .v = 128 },
        [RELIC_CROSS] = { .u = 32, .v = 128 },
        [RELIC_TAROT_CARD] = { .u = 48, .v = 128 },
        [RELIC_HIGH_JUMP] = { .u = 0, .v = 144 }
    };

    if ( ++relicObject->current_frame >= 6) {
        relicObject->current_frame = 0;
        if (relicObject->vertical_direction > 0) {
             relicObject->offset_y++;
             if (relicObject->offset_y >= 10)
                relicObject->vertical_direction = -1;
        } else {
            relicObject->offset_y--;
            if (relicObject->offset_y == 0)
                relicObject->vertical_direction = 1;
        }
    }

    int32_t screen_x = (int32_t)relicObject->x - cam_x;
    int32_t screen_y = relicObject->y - relicObject->offset_y - cam_y;

    RectangleCollisioner collisioner = {
        .x = (float)x,
        .y = (float)y,
        .width = (float)width,
        .height = (float)height
    };

    RectangleCollisioner relic_hit_box = {
        .x = (float)screen_x,
        .y = (float)screen_y,
        .width = 16.0f,
        .height = 16.0f
    };

    if (check_collision_AABB(collisioner, relic_hit_box))
    {
        relicObject->active = 0;
        return 1;
    }
    
    render_sprt(relics_tim, screen_x, screen_y, 16, 16, relic_sprites[relicObject->type].u, relic_sprites[relicObject->type].v, 128, 128, 128);

    return 0;
}


int16_t misc_render_in_camera(MiscObject *miscObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height) {
    SPRITE_OBJECT misc_sprites[MISC_SPRITES] = {
        [0] = { .u = 0, .v = 160 },
        [1] = { .u = 16, .v = 160 }
    };

    int32_t sprite_index = (int32_t)miscObject->type - MISC_CANDLE;
    if (sprite_index < 0 || sprite_index >= MISC_SPRITES)
        return 0;

    if ( ++miscObject->current_frame >= 4 ) {
        miscObject->current_frame = 0;
        miscObject->horizontal_direction = miscObject->horizontal_direction ? 0 : 1;
    }

    int32_t screen_x = (int32_t)miscObject->x - cam_x;
    int32_t screen_y = miscObject->y - miscObject->offset_y - cam_y;
    
    render_poly_ft4(relics_tim, screen_x, screen_y, 16, 16, misc_sprites[sprite_index].u, misc_sprites[sprite_index].v, 128, 128, 128, miscObject->horizontal_direction, 0);

    return 0;
}