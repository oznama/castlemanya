#include "utils.h"

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t) {
    RGBColor resultado;
  
    resultado.r = (c1.r + t * (c2.r - c1.r));
    resultado.g = (unsigned char)(c1.g + t * (c2.g - c1.g));
    resultado.b = (unsigned char)(c1.b + t * (c2.b - c1.b));
    
    return resultado;
}

void update_animation(SPRITE_OBJECT *anim) {
    anim->anim_timer++;

    if (anim->anim_timer >= anim->anim_speed) {
        anim->anim_timer = 0;
        anim->current_frame++;

        // Hacer bucle (loop) al llegar al final
        if (anim->current_frame >= anim->total_frames) {
            anim->current_frame = 0;
        }
    }
}