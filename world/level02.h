#pragma once

#include "../helpers/level/level_generator.h"
#include "../gameplay/many.h"

#define LEVEL_02_ROOMS 3

void level_02_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme);
void level_02_render(uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr);
