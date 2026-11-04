#pragma once

#include <stdint.h>

#include "../../engine/game_state.h"

GameState demo_game_update(uint16_t held_buttons, uint16_t pressed_buttons);
void init_actions_player(uint16_t held_buttons, uint16_t pressed_buttons);
void reset_demo_game(uint16_t pressed_buttons, uint8_t reset_game_state);
void action_block(uint16_t held_buttons);
void action_change_posture(uint16_t pressed_buttons);
void action_crouch(uint16_t held_buttons);
void action_jump(uint16_t pressed_buttons);
void action_move(uint16_t held_buttons);
void action_back_slash(uint16_t pressed_buttons);
void action_attack(uint16_t pressed_buttons);
void check_enemy_collision(void);
void demo_game_render(void);
