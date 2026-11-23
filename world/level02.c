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