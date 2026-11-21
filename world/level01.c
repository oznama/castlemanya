#include "level01.h"

// TODO Pasar estas funciones al level_generator
// TODO Ajustar la colision para efecto escalera
void build_leader_left(
    uint16_t tx, uint16_t ty,
    uint16_t lower_w, uint16_t lower_h ,
    uint8_t steps, uint16_t base_w,
    uint8_t with_base, uint8_t with_border,
    DynamicLevel *level, const LevelThemeTextures *theme);

void build_leader_right(
    uint16_t tx, uint16_t ty,
    uint16_t lower_w, uint16_t lower_h ,
    uint8_t steps, uint16_t base_w,
    uint8_t with_base, uint8_t with_border,
    DynamicLevel *level, const LevelThemeTextures *theme);

void level_01_builder(uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme)
{
    // if (currentRoom > LEVEL_01_ROOMS) {
    //     return;
    // }
    
    // for(int i = 1; i<=LEVEL_01_ROOMS; i++) {
    //     if ( i == currentRoom ) {

    //     }
    // }

    if (currentRoom == 1) {

    } else if (currentRoom == 2) {
        build_leader_left(2, 10, 5, 1, 3, 2, 1, 1, level, theme);
        build_leader_right(level->width-2, 10, 5, 1, 3, 2, 1, 1, level, theme);
        level_add_platform(level, level->width/2-5, 12, 10, theme->pltfrml_tile, theme->pltfrmf_tile, theme->pltfrmr_tile, 1);
        build_leader_right(level->width/2, 25, 11, 1, 6, 3, 1, 0, level, theme);
        build_leader_left(level->width/2, 25, 15, 1, 6, 3, 1, 1, level, theme);
    } else if ( currentRoom == LEVEL_01_ROOMS ) {
        
    }
}


void build_leader_left(uint16_t tx, uint16_t ty,
    uint16_t lower_w, uint16_t lower_h ,
    uint8_t steps, uint16_t base_w,
    uint8_t with_base, uint8_t with_border,
    DynamicLevel *level, const LevelThemeTextures *theme) {
    
    /*
     * Base real es
     * ancho de la plataforma baja 
     * ,as escalones 
     * mas ancho de plataforma alta
     * mas borde de plataforma baja
     */
    // Plataforma base
    uint16_t base_with = lower_w + steps + base_w;
    level_add_wall(level, tx, ty-1, base_with, lower_h, theme->filled_tile, 0, theme->filled_tile, with_base, theme->hillb_tile);
    
    // Inicio de plataforma (parte baja)
    uint16_t lower_tx = tx + base_w + steps;
    uint16_t real_ty = with_base ? ty - 2 : ty - 1;
    level_add_wall(level, lower_tx, real_ty, lower_w - with_border, 1, theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile); // Plataforma
    if (with_border)
        level_add_wall(level, lower_tx + lower_w - 1, real_ty, 1, 1, theme->filled_tile, 1, theme->hillr_tile, 0, theme->filled_tile); // Borde

    // Escaleras
    for (int16_t i = steps -1, j = 0; i >= 0 ; i--, j++) {
        level_add_wall(level, tx + base_w + i, real_ty--, 1, 1+j, theme->filled_tile, 1, theme->stairwaybl_tile, 0, theme->filled_tile); // Relleno
        level_add_wall(level, tx + base_w + i, real_ty, 1, 1, theme->stairwayl_tile, 1, theme->stairwayl_tile, 1, theme->filled_tile); // Escalon
    }
    
    // Final de plataforma (parte alta)
    level_add_wall(level, tx, real_ty, base_w, steps + 1, theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile);
}

void build_leader_right(uint16_t tx, uint16_t ty,
    uint16_t lower_w, uint16_t lower_h ,
    uint8_t steps, uint16_t base_w,
    uint8_t with_base, uint8_t with_border,
    DynamicLevel *level, const LevelThemeTextures *theme) {

    // Plataforma base
    uint16_t base_with = lower_w + steps + base_w;
    level_add_wall(level, tx - base_with, ty-1, base_with, lower_h, theme->filled_tile, 0, theme->filled_tile, with_base, theme->hillb_tile);
    
    // Inicio de plataforma (parte baja)
    uint16_t lower_tx = tx + base_w + steps;
    uint16_t real_ty = with_base ? ty - 2 : ty - 1;
    level_add_wall(level, tx - base_with + with_border, real_ty, lower_w - with_border, 1, theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile); // Plataforma
    if (with_border)
        level_add_wall(level, tx - base_with, real_ty, 1, 1, theme->filled_tile, 1, theme->hilll_tile, 0, theme->filled_tile); // Borde

    uint16_t aux_x = tx - base_with + lower_w;
    // Escaleras
    for (int16_t i = 0; i < steps; i++) {
        level_add_wall(level, aux_x + i, real_ty--, 1, 1+i, theme->filled_tile, 1, theme->stairwaybr_tile, 0, theme->filled_tile); // Relleno
        level_add_wall(level, aux_x + i, real_ty, 1, 1, theme->stairwayr_tile, 1, theme->stairwayr_tile, 1, theme->filled_tile); // Escalon
    }
    
    // Final de plataforma (parte alta)
    level_add_wall(level, aux_x + steps, real_ty, base_w, steps + 1, theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile);
}