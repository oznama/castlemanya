#include "level01.h"

void level_01_builder(DynamicLevel *level, const LevelThemeTextures *theme)
{
    // Pintando barda de final para simular entrada
    level_add_wall(level, level->width-2, 0, level->height - 8, theme->wall_tile);
    level_add_wall(level, level->width-1, 0, level->height - 8, theme->wall_tile);
}