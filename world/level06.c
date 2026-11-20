#include "level06.h"

void level_06_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    if (currentRoom == 1) {
        for(int i = 1; i<=level->width; i++) {
            int posicion = i % 10;
            if (posicion >= 7 || posicion == 0) {
                continue;
            }
            level_add_wall(level, i, level->height-3, 3, theme->ceiling_tile);
        }
    } else if (currentRoom == 2) {
        uint8_t alternate;
        for(int i = 20; i<=level->height - 5;) {
            if (alternate)
                level_add_platform(level, 10, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            else {
                level_add_platform(level, 20, i, 5, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
            }
            i+=10;
            alternate ^= 1;
        }
    }

}