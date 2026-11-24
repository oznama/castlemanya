#pragma once

#include "level_generator.h"
#include "../../gameplay/relic.h"

#define LEVELS_COUNT 8
#define LEVEL_MAX_ROOMS 10

typedef struct
{
    char tilename[20];
    char bgname[BACKGROUND_MAX][20];
    uint16_t width_tiles, height_tiles;
    LevelSkipBorder skip_border;
    LEVEL_ORIENTATION level_orientation;
    RelicObject relicObject;
} LEVEL_DATA;

typedef struct
{
    LevelDoor enter_door;
    LevelDoor exit_door;
    LEVEL_DATA level_data[LEVEL_MAX_ROOMS];
    uint8_t rooms; // Max 10
} LEVEL_WORLD;

extern LEVEL_WORLD LEVELS_INFO[LEVELS_COUNT];

void level_draw(uint8_t currentLevel, uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme);