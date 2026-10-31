#pragma once

#include <stdint.h>

#include "../engine/game_state.h"

GameState demo_game_update(uint16_t held_buttons, uint16_t pressed_buttons);
void action_block(uint16_t held_buttons);
void action_change_posture(uint16_t pressed_buttons);
void action_crouch(uint16_t held_buttons);
void action_jump(uint16_t pressed_buttons);
void action_move(uint16_t held_buttons);
void demo_game_render(void);
