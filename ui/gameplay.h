#pragma once

#include <stdint.h>

typedef struct {
    char tilename[20];
    char bgname[20];
} LEVEL_DATA;

static const LEVEL_DATA LEVELS_INFO[4] = {
    {.tilename = "TLSTCAS", .bgname = "BGCASTLE"},
    {.tilename = "TLSTCLD", .bgname = "BGCOLD"},
    {.tilename = "TLSTEXT", .bgname = "BGEXT"},
    {.tilename = "TLSTGRN", .bgname = "BGGROUND"}
};

void gameplay_main(uint8_t currentLevel);