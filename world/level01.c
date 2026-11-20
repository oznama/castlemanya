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

    if (currentRoom == 1) {

    } else if (currentRoom == 2) {
        // level_add_wall(level, 0, 5, level->height - 8, theme->wall_tile);
        // level_add_wall(level, 1, 5, level->height - 8, theme->wall_tile);

        level_add_platform(level, 2, 5, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);

        uint8_t alternate;
        for(int i = 20; i<=level->height - 5;) {
            if (alternate)
                level_add_platform(level, 10, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            else {
                level_add_platform(level, 20, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
            i+=4;
            alternate ^= 1;
        }

        level_add_platform(level, level->width-12, 5, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);

        // level_add_wall(level, level->width-2, 5, level->height - 45, theme->wall_tile);
        // level_add_wall(level, level->width-1, 5, level->height - 45, theme->wall_tile);
    } else if ( currentRoom == LEVEL_01_ROOMS ) {
        // Pintando barda de final para simular entrada
        // level_add_wall(level, level->width-2, 0, level->height - 8, theme->wall_tile);
        // level_add_wall(level, level->width-1, 0, level->height - 8, theme->wall_tile);
    }
}