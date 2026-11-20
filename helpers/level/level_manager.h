#pragma once

#include "level_generator.h"

#define LEVELS_COUNT 8
#define LEVEL_MAX_ROOMS 10

typedef struct
{
    uint16_t width_tiles, height_tiles;
    LEVEL_ORIENTATION level_orientation;
} LEVEL_DATA;

typedef struct
{
    char tilename[20];
    char bgname[BACKGROUND_MAX][20];
    LevelSkipBorder skip_border;
    LevelDoor enter_door;
    LevelDoor exit_door;
    LEVEL_DATA level_data[LEVEL_MAX_ROOMS];
    uint8_t rooms; // Max 10
} LEVEL_WORLD;

extern const LEVEL_WORLD LEVELS_INFO[LEVELS_COUNT];

void level_draw(uint8_t currentLevel, uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme);