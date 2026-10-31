#pragma once

#include <stdint.h>

#include "../engine/game_state.h"

GameState main_menu_update(uint16_t pressed_buttons);
void main_menu_render(void);
