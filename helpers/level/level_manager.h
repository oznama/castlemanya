#include "level_generator.h"

typedef struct
{
    uint16_t width_tiles, height_tiles;
    char tilename[20];
    char bgname[BACKGROUND_MAX][20];
    LevelSkipBorder skip_border;
    LevelDoor enter_door;
    LevelDoor exit_door;
} LEVEL_DATA;

void level_draw(uint8_t currentLevel, DynamicLevel *level, const LevelThemeTextures *theme);