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