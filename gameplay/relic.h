#pragma once

#include <stdint.h>

typedef enum
{
    RELIC_VAMPIRE_KILLER = 1,
    RELIC_DOBLE_JUMP = 2,
    RELIC_CROSS = 3,
    RELIC_TAROT_CARD = 4,
    RELIC_HIGH_JUMP = 5
} RELIC;

typedef struct
{
    RELIC relic;
    int32_t x;
    int32_t y;
    uint8_t current_frame;
    int8_t offset_y;
    int8_t vertical_direction;
    uint8_t active;
} RelicObject;


void relic_init(void);
uint8_t relic_render_in_camera(RelicObject *relicObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height);