#include <psxpad.h>
#include <stdio.h>

#include "test_area.h"
#include "../../graphics/renderer.h"

#define SCREEN_XRES 320
#define SCREEN_YRES 240

GameState test_area_update(uint16_t held_buttons, uint16_t pressed_buttons) {
    if (pressed_buttons & PAD_SELECT) {
        return GAME_STATE_MENU;
    } else if (pressed_buttons & PAD_CROSS) {
        printf("X pressed\n");
    }

    return GAME_STATE_TESTS;

}

void test_area_render(void) {
    renderer_draw_text(SCREEN_XRES - 100, 8, 0, "TEST AREA"); // Draw test area label
}