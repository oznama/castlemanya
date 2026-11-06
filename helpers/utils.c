#include <stdint.h>

#include "utils.h"
#include "../engine/input.h"
#include "../ui/demo/demo.h"


static void show_loading_page() {

}

static void load_level_resources(uint8_t currentLevel) {

}

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t) {
    RGBColor resultado;
  
    resultado.r = (c1.r + t * (c2.r - c1.r));
    resultado.g = (unsigned char)(c1.g + t * (c2.g - c1.g));
    resultado.b = (unsigned char)(c1.b + t * (c2.b - c1.b));
    
    return resultado;
}

void load_level(uint8_t currentLevel) {
    // Mostrar pantalla estatica de Cargando
    show_loading_page();

    load_level_resources(currentLevel);

    while(1) {
        uint16_t held_buttons;
        uint16_t pressed_buttons;
        input_read(&held_buttons, &pressed_buttons);

        demo_game_update(held_buttons, pressed_buttons);
        demo_game_render();
    }
}