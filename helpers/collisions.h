#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Comprobación de colisión AABB usando aritmética de enteros (sin floats, optimizado para MIPS R3000A) */
static inline bool check_collision_box_int(int32_t x1, int32_t y1, int32_t w1, int32_t h1,
                                           int32_t x2, int32_t y2, int32_t w2, int32_t h2)
{
    return (x1 < x2 + w2) && (x1 + w1 > x2) &&
           (y1 < y2 + h2) && (y1 + h1 > y2);
}
