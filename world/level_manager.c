#include <stdio.h>
#include "level_manager.h"

#include "level01.h"

void level_draw(uint8_t currentLevel, DynamicLevel *level, const LevelThemeTextures *theme) {
    printf("Drawing level: %d\n", currentLevel);
    switch (currentLevel)
    {
    case 1:
        printf("Builder level 1\n");
        level_01_builder(level, theme);
        break;
    
    default:
        break;
    }
}