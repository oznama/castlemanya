#pragma once

#include <stdint.h>

void input_init(void);
void input_read(uint16_t *held_buttons, uint16_t *pressed_buttons);
