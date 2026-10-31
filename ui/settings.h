#pragma once

#include <stdint.h>

#include "../engine/game_state.h"

GameState settings_update(uint16_t pressed_buttons);
void settings_render(void);
