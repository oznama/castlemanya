#include "level06.h"

void level_06_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    if (currentRoom == 1) {
        
    } else if (currentRoom == 2) {
        uint8_t alternate;
        for(int i = 30; i<=level->height - 5;) {
            if (alternate)
                level_add_platform(level, 2, i, 8, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            else {
                level_add_platform(level, 20, i, 8, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
            i+=10;
            alternate ^= 1;
        }
        level_add_platform(level, 20, 10, 8, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
    }

}
#include "../helpers/level/level_manager.h"
#include "../helpers/collisions.h"

void level_06_render(uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr)
{
    if (currentRoom < 1 || currentRoom > LEVEL_06_ROOMS)
        return;

    LEVEL_DATA *room_data = &LEVELS_INFO[5].level_data[currentRoom - 1];

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
