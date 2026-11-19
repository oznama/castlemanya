#pragma once

#include <stdint.h>

#include "../helpers/level_generator.h"

typedef struct
{
    uint16_t width_tiles, height_tiles;
    char tilename[20];
    char bgname[BACKGROUND_MAX][20];
    LevelDoor enter_door;
    LevelDoor exit_door;
} LEVEL_DATA;

static const LEVEL_DATA LEVELS_INFO[] = {
    {.width_tiles = 128, .height_tiles = 15, .tilename = "TLSTENT", .bgname = {"BGENT1", "BGENT2", "BGENT3", "BGENT4"}, .enter_door = {.x = 20, .y = 111, .rotateu = 0, .open = 0, .active = 1}, .exit_door = {.x = 2016, .y = 111, .rotateu = 1, .open = 0, .active = 1}},
    {.width_tiles = 64, .height_tiles = 15, .tilename = "TLSTCSHW", .bgname = {"BGCSTHW1", "BGCSTHW2", "BGCSTHW3", "BGCSTHW4"}, .enter_door = {.x = 20, .y = 111, .rotateu = 0, .open = 0, .active = 1}, .exit_door = {.x = 980, .y = 111, .rotateu = 1, .open = 0, .active = 1}},
    {.width_tiles = 40, .height_tiles = 25, .tilename = "TLSTCAS", .bgname = {"BGCASTLE"}},
    {.width_tiles = 40, .height_tiles = 25, .tilename = "TLSTCLD", .bgname = {"BGCOLD"}},
    {.width_tiles = 40, .height_tiles = 25, .tilename = "TLSTEXT", .bgname = {"BGEXT"}},
    {.width_tiles = 40, .height_tiles = 25, .tilename = "TLSTGRN", .bgname = {"BGGROUND"}}};

void gameplay_main(uint8_t currentLevel);