#include "level01.h"

void level_01_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    /* Esto es para dibujar columnas sin colision cada 10 tiles en x */
    // TileTextureDef tile = {
    //     .u = theme->wall_tile.u,
    //     .v = theme->wall_tile.v,
    //     .collision = TILE_COLL_NONE
    // };
    switch (currentRoom)
    {
    case 1: // 40 x 15 tiles
        /* code */
        break;
    case 2: // 40 x 25
        level_add_stairway(level, theme, 2, 14, 5, 1, 7, 2, 1, 1, STAIRWAY_LEFT);
        level_add_stairway(level, theme, level->width - 2, 14, 5, 1, 7, 2, 1, 1, STAIRWAY_RIGHT);
        // level_add_platform(level, level->width / 2 - 5, 12, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_stairway(level, theme, level->width / 2, 25, 3, 1, 7, 3, 0, 0, STAIRWAY_RIGHT);
        level_add_stairway(level, theme, level->width / 2, 25, 15, 1, 7, 3, 0, 0, STAIRWAY_LEFT);
        // TODO Agregar a personaje en habitacion 3 que habilitara el acceso pagando 500 pesos
        // level_add_wall(level, 2, level->height - 1, 5, 1, theme->wall_tile, 0, theme->wall_tile, 0, theme->wall_tile);
        break;
    case 3: // 20 x 15
        level_add_wall(level, level->width - 6, level->height - 5, 4, 2, theme->wall_tile, 0, theme->hillf_tile, 0, theme->hillb_tile);
        break;
    case 4: // 60 x 25
        level_add_platform(level, 30, 5, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_wall(level, 30, 10, 22, 3, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 26, 1, 4, 12, theme->filled_tile, 0, theme->hillf_tile, 1, theme->hillb_tile);
        for ( int i = 10; i < level->height-5; i+=5 ) {
            level_add_platform(level, level->width - 4, i, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        }
        level_add_platform(level, 2, 7, 11, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_platform(level, 15, 12, 11, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_platform(level, 2, 17, 11, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        break;
    case 5: // 25 x 59
        level_add_wall(level, 7, 6, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 2, 11, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 7, 16, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 2, 21, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 7, 26, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 2, 31, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 7, 36, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 2, 41, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 7, 46, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, 2, 51, 16, 2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        break;
    case 6:
        /* code */
        break;
    case 7:
        /* code */
        break;
    case 8:
        /* code */
        break;
    case 9:
        /* code */
        break;
    case LEVEL_01_ROOMS: // 40 x 15
        /* code */
        break;
    default:
        break;
    }
}