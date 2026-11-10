#include "utils.h"

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t) {
    RGBColor resultado;
  
    resultado.r = (c1.r + t * (c2.r - c1.r));
    resultado.g = (unsigned char)(c1.g + t * (c2.g - c1.g));
    resultado.b = (unsigned char)(c1.b + t * (c2.b - c1.b));
    
    return resultado;
}

uint8_t update_animation(SPRITE_OBJECT *anim) {
    if (!anim || anim->total_frames <= 1) {
        return 1; // Si no tiene frames para animar, se considera completada
    }

    // Si no tiene direccion asignada o arranca en 0, inicializar hacia adelante
    if (anim->anim_dir == 0) {
        anim->anim_dir = 1;
    }

    anim->anim_timer++;

    if (anim->anim_timer >= anim->anim_speed) {
        anim->anim_timer = 0;

        // Calculamos el siguiente fotograma con signo (int16_t) para evitar
        // desbordamientos por debajo de cero en uint8_t
        int16_t next_frame = (int16_t)anim->current_frame + anim->anim_dir;

        if (next_frame >= (int16_t)anim->total_frames - 1) {
            anim->current_frame = anim->total_frames - 1;
            anim->anim_dir = -1; // Llego al final -> ahora retrocede
            return 1; // <<-- Notifica que completo el recorrido de inicio a fin
        } else if (next_frame <= 0) {
            anim->current_frame = 0;
            anim->anim_dir = 1;  // Llego al inicio -> ahora avanza
            return 1; // <<-- Notifica que regreso al inicio
        } else {
            anim->current_frame = (uint8_t)next_frame;
        }
    }
    return 0; // Animacion aun en curso
}
