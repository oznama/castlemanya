#pragma once

#include <stdint.h>
#include "../helpers/level/level_generator.h"

typedef enum
{
    RELIC_VAMPIRE_KILLER = 1,
    RELIC_DOBLE_JUMP = 2,
    RELIC_CROSS = 3,
    RELIC_TAROT_CARD = 4,
    RELIC_HIGH_JUMP = 5,
    MISC_CANDLE = 6,
    MISC_CASH = 7
} MISC_TYPE;

typedef struct
{
    // Estados iniciales
    MISC_TYPE initial_type;
    int32_t initial_y;
    int32_t velocity_y;
    uint8_t initial_state_saved;
    uint8_t falling;
    uint8_t initial_r;
    uint8_t initial_g;
    uint8_t initial_b;
    // Estados base
    MISC_TYPE type;
    int32_t x; // Posicion en x
    int32_t y; // Posicion en y
    uint8_t r; // Tono red
    uint8_t g; // Tono green
    uint8_t b; // Tono blue
    uint8_t current_frame;
    int8_t offset_y;
    int8_t vertical_direction;
    uint8_t horizontal_direction;
    uint8_t active;
    uint8_t value;
} MiscObject;


void misc_init(void);
uint8_t relic_render_in_camera(MiscObject *relicObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height);
int16_t misc_render_in_camera(MiscObject *miscObject, int32_t cam_x, int32_t cam_y, int32_t x, int32_t y, int32_t width, int32_t height);
void misc_update(MiscObject *miscObject, const DynamicLevel *level);