#include "level02.h"

void level_02_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    // if (currentRoom > LEVEL_01_ROOMS) {
    //     return;
    // }

    // for(int i = 1; i<=LEVEL_01_ROOMS; i++) {
    //     if ( i == currentRoom ) {

    //     }
    // }

    if (currentRoom == 1)
    {
    }
    else if (currentRoom == 2)
    {
        // int first = level->height - 3;
        // int aux[first];
        // for (int i = 0; i<first; i++){
        //     aux[i] = 3 + i;
        // }
        // for (int i = first - 1; i>0; i-=5) {
        //     if ( i % 2 == 0) {
        //         level_add_stairway(level, theme, 0, aux[i], 5, 1, 3, 4, 1, 1, STAIRWAY_LEFT);
        //     } else {
        //         level_add_stairway(level, theme, level->width - 2, aux[i], 5, 1, 3, 4, 1, 1, STAIRWAY_RIGHT); 
        //     }
        // }
        uint16_t h = level->height - 2;
        uint16_t s = 6;
        level_add_stairway(level, theme, level->width - 2, h, 3, 1, s, 5, 0, 0, STAIRWAY_RIGHT);
        h = h - s - 3;
        s = 3;
        level_add_stairway(level, theme, 2, h, 9, 1, s, 5, 1, 1, STAIRWAY_LEFT);
        h = h - s - 3;
        level_add_stairway(level, theme, level->width - 2, h, 8, 1, s, 5, 1, 1, STAIRWAY_RIGHT);
        h = h - s - 3;
        level_add_stairway(level, theme, 2, h, 9, 1, s, 5, 1, 1, STAIRWAY_LEFT);
        h = h - s - 3;
        level_add_stairway(level, theme, level->width - 2, h, 8, 1, s, 5, 1, 1, STAIRWAY_RIGHT);

    }
    else if (currentRoom == LEVEL_02_ROOMS)
    {
    }
}
#include "../helpers/level/level_manager.h"
#include "../helpers/collisions.h"

void level_02_render(uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr)
{
    if (currentRoom < 1 || currentRoom > LEVEL_02_ROOMS)
        return;

    LEVEL_DATA *room_data = &LEVELS_INFO[1].level_data[currentRoom - 1];

    /* Obtener coordenadas y dimensiones de colisión de Many (espacio de mundo) */
    int32_t many_box_x = (int32_t)many_ptr->x;
    int32_t many_box_y = (int32_t)((many_ptr->y >> 8) + MANY_COLLISION_HEIGHT - 16);
    int32_t many_box_w = MANY_COLLISION_WIDTH;
    int32_t many_box_h = 16;

    /* Obtener hitbox del látigo de Many */
    int32_t whip_x = 0, whip_y = 0, whip_w = 0, whip_h = 0;
    uint8_t whip_active = many_get_whip_hitbox(&whip_x, &whip_y, &whip_w, &whip_h);

    /* Renderizar y evaluar colisiones de los misceláneos de la sala si existen */
    for (int16_t i = 0; i < ARRAY_TWENTY; i++)
    {
        MiscObject *misc = &room_data->miscObjects[i];
        if (!misc->active)
            continue;

        if (misc->type < MISC_CANDLE)
        {
            uint8_t relic_touched = relic_render_in_camera(
                misc, cam_x, cam_y,
                many_box_x, many_box_y, many_box_w, many_box_h);

            if (relic_touched)
            {
                many_collect_relic(many_ptr, misc->type);
            }
        }
        else
        {
            misc_render_in_camera(
                misc, cam_x, cam_y,
                many_box_x, many_box_y, many_box_w, many_box_h);

            if (whip_active)
            {
                int32_t misc_world_y = misc->y - misc->offset_y;
                if (check_collision_box_int(whip_x, whip_y, whip_w, whip_h,
                                            misc->x, misc_world_y, 16, 16))
                {
                    misc->active = 0;
                }
            }
        }
    }
}
