#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h> // Este solo esta incluido por el free
#include <psxgpu.h>
#include <psxpad.h>

#include "main_menu.h"
#include "../../engine/input.h"
#include "../../graphics/graphic.h"
#include "../../helpers/utils.h"

// TODO Replace static load by load from CD
extern u_long	castlemanya_logo[];

GameState main_menu_display() {

    GameState currentState = GAME_STATE_MENU;

    graphic_init();
    input_init();

    // Menu infinite till option selected
    uint8_t menuActive = 1;

    // Selector vars
    uint8_t option_count = 3;
    uint8_t selected_y;
    uint8_t selected_option;
    // Rect selector animation vars
    RGBColor SELECTOR_COLOR_1 = { 255, 0, 0 };
    RGBColor SELECTOR_COLOR_2 = { 68, 12, 8 };
    RGBColor selector_color = SELECTOR_COLOR_1;
    float factor = 0.0f;
    float velocity = 0.02f;
    int direction = 1;

    // Load logo image
    TIM_IMAGE tim;
    GetTimInfo(castlemanya_logo, &tim);
    LoadImage(tim.prect, tim.paddr);
	DrawSync(0);
	if ( tim.mode & 0x8 ) {
		LoadImage(tim.crect, tim.caddr);
		DrawSync(0);
	}
    free(castlemanya_logo);
    
    // Main cycle
    while(menuActive) {
        uint16_t held_buttons; // Not used in this page
        uint16_t pressed_buttons;
        input_read(&held_buttons, &pressed_buttons);

        // Rect selector
        selector_color = interpolateColor(SELECTOR_COLOR_1, SELECTOR_COLOR_2, factor);
        factor += velocity * direction;
        if (factor >= 1.0f) {
            factor = 1.0f;
            direction = -1;
        } else if (factor <= 0.0f) {
            factor = 0.0f;
            direction = 1;
        }

        // Buttons functions
        if (pressed_buttons & (PAD_UP | PAD_DOWN)) {
            if (pressed_buttons & PAD_UP && selected_option > 0)
                selected_option--;
            else if (pressed_buttons & PAD_DOWN && selected_option < option_count - 1)
                selected_option++;
        }
        if (pressed_buttons & (PAD_CROSS | PAD_START)) {
            menuActive = 0; // Break infinite while
            currentState = GAME_STATE_LOADING;
        }

        // Render menu
        render_sprt(tim, 32, 32, 128, 128, 128);
        render_text(112, 120, 0, "START GAME");
        render_text(112, 140, 0, "CONTINUE");
        render_text(112, 160, 0, "SETTINGS");
        // Selector
        switch (selected_option) {
            case 0:
                selected_y = 115;
                break;
            case 1:
                selected_y = 135;
                break;
            case 2:
                selected_y = 155;
                break;
            default:
                selected_y = 115;
                break;
        }
        render_rect(76, selected_y, 168, 20, selector_color.r, selector_color.g, selector_color.b); // Dibuja un rectángulo detrás de la opción seleccionada
        render_text(92, selected_y + 4, 0, ">"); // Dibuja un símbolo de flecha delante de la opción seleccionada
        
        graphic_display();
    }

    return currentState;

}