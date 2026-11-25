#pragma once

#include <stdint.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBColor;

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t);

uint16_t drop_money(void);