#include "level01.h"

void level_01_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    /* Esto es para dibujar columnas sin colision cada 10 tiles en x */
    // TileTextureDef tile = {
    //     .u = theme->wall_tile.u,
    //     .v = theme->wall_tile.v,
    //     .collision = TILE_COLL_NONE
    // };

    if (currentRoom == 1) // 40 x 15 tiles
    {
        // Agrega columnas de fondo al nivel
        // for(uint32_t i = 10; i<level->width; i+=10) {
        //     level_fill_rect(level, i, level->height - 9, 1, 6, tile);
        // }
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