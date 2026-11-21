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

    if (currentRoom == 1)
    {
    }
    else if (currentRoom == 2)
    {
        level_add_stairway(level, theme, 2, 10, 5, 1, 3, 2, 1, 1, STAIRWAY_LEFT);
        level_add_stairway(level, theme, level->width - 2, 10, 5, 1, 3, 2, 1, 1, STAIRWAY_RIGHT);
        level_add_platform(level, level->width / 2 - 5, 12, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        level_add_stairway(level, theme, level->width / 2, 25, 10, 1, 7, 3, 0, 0, STAIRWAY_RIGHT);
        level_add_stairway(level, theme, level->width / 2, 25, 15, 1, 7, 3, 0, 0, STAIRWAY_LEFT);
    }
    else if (currentRoom == LEVEL_01_ROOMS)
    {
    }
}