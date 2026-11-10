#include <stdint.h>

typedef struct {
	uint32_t w; // w: ancho del sprite
	uint32_t h; // h: altura del sprite
	uint32_t u; // u: posicion en coordenada x
	uint32_t v; // v: posicion en coordenada y
	uint8_t current_frame; // Fotograma actual
	uint8_t total_frames; // Cantidad de fotogramas en la animacion
	uint8_t anim_timer; // Temporizador para controlar los FPS de la animacion
	uint8_t anim_speed; // Cuantos fotogramas de la PS1 deben pasar para cambiar de frame (ej. 6)
} SPRITE_OBJECT;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBColor;

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t);

void update_animation(SPRITE_OBJECT *anim);