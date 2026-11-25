#include "misc.h"

#include <psxgpu.h>

#include "../engine/cd_loader.h"
#include "../graphics/graphic.h"
#include "../graphics/sprite.h"
#include "../helpers/collisions.h"

#define RELICS_SPRITES 6
#define MISC_SPRITES 2
#define MISC_CASH_GRAVITY 1
#define MISC_CASH_MAX_FALL_SPEED 4

CD_TEXTURE relics_tim;

void misc_init(void)
{
    cd_load_image("\\MANYREL.TIM", &relics_tim);
}

uint8_t relic_render_in_camera(MiscObject *relicObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height)
{
    SPRITE_OBJECT relic_sprites[RELICS_SPRITES] = {
        // El 0 se omite
        [RELIC_VAMPIRE_KILLER] = {.u = 0, .v = 128},
        [RELIC_DOBLE_JUMP] = {.u = 16, .v = 128},
        [RELIC_CROSS] = {.u = 32, .v = 128},
        [RELIC_TAROT_CARD] = {.u = 48, .v = 128},
        [RELIC_HIGH_JUMP] = {.u = 0, .v = 144}};

    if (++relicObject->current_frame >= 6)
    {
        relicObject->current_frame = 0;
        if (relicObject->vertical_direction > 0)
        {
            relicObject->offset_y++;
            if (relicObject->offset_y >= 10)
                relicObject->vertical_direction = -1;
        }
        else
        {
            relicObject->offset_y--;
            if (relicObject->offset_y == 0)
                relicObject->vertical_direction = 1;
        }
    }

    int32_t screen_x = (int32_t)relicObject->x - cam_x;
    int32_t screen_y = relicObject->y - relicObject->offset_y - cam_y;

    /* Detección de colisión usando aritmética entera pura en coordenadas de mundo */
    int32_t relic_world_y = relicObject->y - relicObject->offset_y;
    if (check_collision_box_int(x, y, width, height,
                                relicObject->x, relic_world_y, 16, 16))
    {
        relicObject->active = 0;
        return 1;
    }

    if (screen_x < SCREEN_XRES &&
    screen_x + 16 > 0 &&
    screen_y < SCREEN_YRES &&
    screen_y + 16 > 0)
    {
        render_sprt(
            relics_tim,
            screen_x,
            screen_y,
            16, 16,
            relic_sprites[relicObject->type].u,
            relic_sprites[relicObject->type].v,
            128, 128, 128);
    }

    return 0;
}

int16_t misc_render_in_camera(MiscObject *miscObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height)
{
    SPRITE_OBJECT misc_sprites[MISC_SPRITES] = {
        [0] = {.u = 0, .v = 160},
        [1] = {.u = 16, .v = 160}};

    int32_t sprite_index = (int32_t)miscObject->type - MISC_CANDLE;
    if (sprite_index < 0 || sprite_index >= MISC_SPRITES)
        return 0;

    // TODO Este efecto no aplica para todos, en este caso aplica para las velas
    if (miscObject->type == MISC_CANDLE && ++miscObject->current_frame >= 4)
    {
        miscObject->current_frame = 0;
        miscObject->horizontal_direction = miscObject->horizontal_direction ? 0 : 1;
    }
    /*
     * TODO Agegar el efecto para el dinero,
     * el cual debe ser caer hacia el suelo,
     * y esto seria aumentar valor en y hasta el limite del suelo
     */

    int32_t screen_x = (int32_t)miscObject->x - cam_x;
    int32_t screen_y = miscObject->y - miscObject->offset_y - cam_y;

    /* Si se pasan dimensiones válidas, comprobar colisión con coordenadas de mundo */
    if (width > 0 && height > 0)
    {
        int32_t misc_world_y = miscObject->y - miscObject->offset_y;
        if (check_collision_box_int(x, y, width, height,
                                    miscObject->x, misc_world_y, 16, 16))
        {
            return 1;
        }
    }

    if (screen_x < SCREEN_XRES &&
    screen_x + 16 > 0 &&
    screen_y < SCREEN_YRES &&
    screen_y + 16 > 0)
    {
        render_poly_ft4(
            relics_tim,
            screen_x, screen_y,
            16, 16,
            misc_sprites[sprite_index].u,
            misc_sprites[sprite_index].v,
            128, 128, 128,
            miscObject->horizontal_direction, 0);
    }

    return 0;
}

void misc_update(MiscObject *miscObject, const DynamicLevel *level)
{
    if (miscObject->type != MISC_CASH || !miscObject->falling)
        return;

    int32_t x = miscObject->x;
    int32_t y = miscObject->y;
    int32_t velocity_x = 0;
    bool grounded = false;

    if (miscObject->velocity_y < MISC_CASH_MAX_FALL_SPEED)
        miscObject->velocity_y += MISC_CASH_GRAVITY;

    level_resolve_entity_collision(
        level, &x, &y, &velocity_x, &miscObject->velocity_y,
        16, 16, -1, &grounded);

    miscObject->x = x;
    miscObject->y = y;

    if (grounded)
    {
        miscObject->velocity_y = 0;
        miscObject->falling = 0;
    }
}