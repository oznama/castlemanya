#include <stdint.h>

typedef struct {
	uint32_t w; // w: ancho del sprite
	uint32_t h; // h: altura del sprite
	uint32_t u; // u: posicion en coordenada x
	uint32_t v; // v: posicion en coordenada y
} SPRITE_OBJECT;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBColor;

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t);