#include "level01.h"

void level_01_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    // if (currentRoom > LEVEL_01_ROOMS) {
    //     return;
    // }

    // for(int i = 1; i<=LEVEL_01_ROOMS; i++) {
    //     if ( i == currentRoom ) {

    //     }
    // }

    if (currentRoom == 1) // 40 x 15 tiles
    {
        /* Esto es para dibujar columnas sin colision cada 10 tiles en x */
        TileTextureDef tile = {
            .u = theme->wall_tile.u,
            .v = theme->wall_tile.v,
            .collision = TILE_COLL_NONE
        };
        for(uint32_t i = 10; i<level->width; i+=10) {
            level_fill_rect(level, i, level->height - 9, 1, 6, tile);
        }
    }
    else if (currentRoom == 2)
    {
        level_add_stairway(level, theme, 2, 10, 5, 1, 3, 2, 1, 1, STAIRWAY_LEFT);
        level_add_stairway(level, theme, level->width - 2, 10, 5, 1, 3, 2, 1, 1, STAIRWAY_RIGHT);
        level_add_platform(level, level->width / 2 - 5, 12, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_stairway(level, theme, level->width / 2, 25, 10, 1, 7, 3, 0, 0, STAIRWAY_RIGHT);
        level_add_stairway(level, theme, level->width / 2, 25, 15, 1, 7, 3, 0, 0, STAIRWAY_LEFT);
    }
    else if (currentRoom == 3)
    {
        level_add_wall(level, level->width - 6, level->height - 5, 4, 2, theme->wall_tile, 0, theme->hillf_tile, 0, theme->hillb_tile);
    }
    else if (currentRoom == LEVEL_01_ROOMS)
    {
    }
}
#include "../helpers/level/level_manager.h"
#include "../helpers/collisions.h"

void level_01_render(uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr)
{
    if (currentRoom < 1 || currentRoom > LEVEL_01_ROOMS)
        return;

    LEVEL_DATA *room_data = &LEVELS_INFO[0].level_data[currentRoom - 1];

    /* Obtener coordenadas y dimensiones de colisión de Many (espacio de mundo) */
    int32_t many_box_x = (int32_t)many_ptr->x;
    int32_t many_box_y = (int32_t)((many_ptr->y >> 8) + MANY_COLLISION_HEIGHT - 16);
    int32_t many_box_w = MANY_COLLISION_WIDTH;
    int32_t many_box_h = 16;

    /* Obtener hitbox del látigo de Many en este frame si está atacando */
    int32_t whip_x = 0, whip_y = 0, whip_w = 0, whip_h = 0;
    uint8_t whip_active = many_get_whip_hitbox(&whip_x, &whip_y, &whip_w, &whip_h);

    /* Renderizar y evaluar colisiones de los misceláneos de la sala */
    for (int16_t i = 0; i < ARRAY_TWENTY; i++)
    {
        MiscObject *misc = &room_data->miscObjects[i];
        if (!misc->active)
            continue;

        /* Si es reliquia (< MISC_CANDLE) */
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
            /* Cualquier otro misc (velas, dinero, etc.) */
            int16_t hit_many = misc_render_in_camera(
                misc, cam_x, cam_y,
                many_box_x, many_box_y, many_box_w, many_box_h);

            /* Comprobar si el látigo de Many golpea la vela/antorcha */
            if (whip_active)
            {
                int32_t misc_world_y = misc->y - misc->offset_y;
                if (check_collision_box_int(whip_x, whip_y, whip_w, whip_h,
                                            misc->x, misc_world_y, 16, 16))
                {
                    misc->active = 0;
                    /* La vela fue destruida por el látigo */
                }
            }
        }
    }
}
