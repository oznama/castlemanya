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
        // level_add_wall(level, level->width - 6, level->height - 5, 4, 2, theme->wall_tile, 0, theme->hillf_tile, 0, theme->hillb_tile);
        break;
    case 4: // 60 x 15
        level_add_platform(level, level->width - 4, 4, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_platform(level, level->width - 4, 8, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);

        // Para crear piso irregular
        // level_add_stairway(level, theme, level->width - 2, level->height - 2, 5, 6, 2, 5, 1, 0, STAIRWAY_RIGHT);
        // level_add_stairway(level, theme, 46, level->height-1, 5, 3, 1, 5, 1, 0, STAIRWAY_RIGHT);
        // level_add_stairway(level, theme, 23, level->height-1, 5, 4, 2, 5, 1, 0, STAIRWAY_LEFT);
        // level_add_stairway(level, theme, 23, level->height-2, 5, 4, 1, 5, 1, 0, STAIRWAY_RIGHT);
        // level_add_stairway(level, theme, 13, level->height-1, 5, 3, 1, 5, 1, 0, STAIRWAY_RIGHT);

        // level_add_wall(level, 30, 10, 22, 3, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        // level_add_wall(level, 26, 1, 4, 12, theme->filled_tile, 0, theme->hillf_tile, 1, theme->hillb_tile);
        // for ( int i = 12; i < level->height - 3; i+=5 ) {
        //     level_add_platform(level, level->width - 4, i, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        // }
        // level_add_platform(level, 2, 7, 11, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        // level_add_platform(level, 15, 12, 11, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        // level_add_platform(level, 2, 17, 11, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        break;
    case 5: // 25 x 59
    {
        const uint16_t wall_width = 16;
        const uint16_t wall_height = 2;
        const uint16_t margin_x = 2;
        const uint16_t row_spacing = 10;

        if (level->width >= wall_width + margin_x * 2)
        {
            const uint16_t right_x = level->width - wall_width - margin_x;

            for (uint16_t row = 0; ; ++row)
            {
                const int32_t y = 6 + row * row_spacing;

                if (y + wall_height > (int32_t)level->height - 6)
                    break;

                const uint16_t x = (row % 2 == 0) ? right_x : margin_x;
                const uint16_t px = (row % 2 == 0) ? 2 : level->width - 4;

                level_add_wall(level, x, (uint16_t)y, wall_width, wall_height,
                            theme->filled_tile, 1, theme->hillf_tile,
                            1, theme->hillb_tile);
                level_add_platform(level, px, (uint16_t)y + 5, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
        }
        break;
    }
    case 6: // 20 x 89
        level_add_platform(level, 2, 6, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        for (int i = 11; i<level->height - 3; i+=5) {
            level_add_platform(level, level->width - 7, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        }
        break;
    case 7: // 30 x 40
        level_add_wall(level, level->width/2, level->height/2, 18, 4, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        level_add_wall(level, level->width/2-4, 6, 4, level->height/2-2, theme->filled_tile, 1, theme->hillf_tile, 1, theme->hillb_tile);
        for (int i = 8; i<level->height - 3; i+=5) {
            if ( i<level->height/2 - 1 ) {
                level_add_platform(level, level->width/2, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
                level_add_platform(level, level->width/2-9, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
            if ( i>level->height/2 - 5) {
                level_add_platform(level, 2, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
            if ( i>level->height/2 + 9) {
                level_add_platform(level, level->width-7, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
        }
        break;
    case 8: // 128 x 64
        level_add_stairway(level, theme, 2, 10, 5, 12, 2, 5, 1, 0, STAIRWAY_LEFT);
        level_add_stairway(level, theme, 14, 20, 40, 2, 10, 50, 1, 1, STAIRWAY_LEFT);

        level_add_platform(level, level->width-11, 23, 3, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);

        level_add_stairway(level, theme, level->width - 2, 32, 51, 4, 2, 60, 1, 1, STAIRWAY_RIGHT);

        level_add_platform(level, 6, 35, 4, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_platform(level, 2, 40, 2, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);

        level_add_stairway(level, theme, 2, 52, 47, 2, 5, 60, 1, 1, STAIRWAY_LEFT);

        level_add_platform(level, level->width-11, 54, 3, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);

        level_add_stairway(level, theme, level->width - 2, level->height-2, 1, 5, 2, 10, 0, 0, STAIRWAY_RIGHT);
        break;
    case 9: // 40 x 15
        level_add_wall(level, 2, level->height - 5, 3, 2, theme->wall_tile, 0, theme->hillf_tile, 0, theme->hillb_tile);
        break;
    case LEVEL_01_ROOMS: // 40 x 15
        /* code */
        break;
    default:
        break;
    }
}