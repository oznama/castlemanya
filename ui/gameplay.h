#pragma once

#include <stdint.h>

#include "../helpers/level/level_generator.h"
#include "../helpers/level/level_manager.h"

#define LEVELS_COUNT (sizeof(LEVELS_INFO) / sizeof(LEVELS_INFO[0]))

void gameplay_main(uint8_t currentLevel);