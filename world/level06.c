#include "level06.h"

void level_06_builder(DynamicLevel *level, const LevelThemeTextures *theme)
{

    for(int i = 1; i<=level->width; i++) {
        int posicion = i % 10;
        if (posicion >= 7 || posicion == 0) {
            continue;
        }
        level_add_wall(level, i, level->height-3, 3, theme->ceiling_tile);
    }

}