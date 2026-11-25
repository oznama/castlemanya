#pragma once

#include "../helpers/level/level_generator.h"
#include "../gameplay/many.h"

#define LEVEL_01_ROOMS 4

void level_01_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme);
void level_01_render(uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr);
