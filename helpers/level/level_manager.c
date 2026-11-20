#include "level_manager.h"

#include "../../world/level01.h"
#include "../../world/level06.h"

void level_draw(uint8_t currentLevel, DynamicLevel *level, const LevelThemeTextures *theme) {
    
    switch (currentLevel)
    {
    case 1:
        level_01_builder(level, theme);
        break;
    case 6:
        level_06_builder(level, theme);
        break;
    default:
        break;
    }
}