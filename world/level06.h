#pragma once

#include "../helpers/level/level_generator.h"
#include "../gameplay/many.h"

#define LEVEL_06_ROOMS 2

void level_06_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme);
void level_06_render(uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr);
