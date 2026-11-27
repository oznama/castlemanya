#pragma once

#include <stdint.h>
#include "enemy.h"

/* Estados del enemigo Murciélago (Bat) */
typedef enum
{
    BAT_STATE_HANGING = 0, /* En reposo colgado en el techo */
    BAT_STATE_WAKING,      /* Transición de reposo a vuelo */
    BAT_STATE_FLYING       /* Vuelo horizontal activo */
} BatState;

/* Carga la textura del murciélago en VRAM */
void bat_init(void);

/* Configura un enemigo con las propiedades específicas del murciélago */
void bat_setup(Enemy *enemy, int32_t x, int32_t y);

/* Actualiza la IA y movimiento del murciélago */
void bat_update(Enemy *enemy, int32_t many_x, int32_t many_y, int32_t level_w, int32_t level_h);

/* Renderiza el murciélago en coordenadas de cámara */
void bat_render(Enemy *enemy, int32_t cam_x, int32_t cam_y);
