#pragma once

#include <stdint.h>

#include "../engine/game_state.h"

GameState demo_game_update(uint16_t pressed_buttons);
void demo_game_render(void);
