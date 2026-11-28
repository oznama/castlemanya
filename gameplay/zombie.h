#pragma once

#include <stdint.h>
#include "enemy.h"

/*
 * Dimensiones del sprite del Zombie:
 * Hoja de 96x96 px (8 sprites de 24x48):
 * - Emergencia (5 frames):
 *   Frame 0: (0, 0)
 *   Frame 1: (24, 0)
 *   Frame 2: (48, 0)
 *   Frame 3: (72, 0)
 *   Frame 4: (0, 48)
 * - Marcha / Caminar (3 frames):
 *   Frame 0: (24, 48)
 *   Frame 1: (48, 48)
 *   Frame 2: (72, 48)
 */
#define ZOMBIE_SPRITE_W 24
#define ZOMBIE_SPRITE_H 48

/* Estados del enemigo Zombie (fiel a Castlevania: Symphony of the Night) */
typedef enum
{
    ZOMBIE_STATE_EMERGING = 0, /* Emergiendo de la tierra (invulnerable / sin hitbox activo) */
    ZOMBIE_STATE_WALKING,      /* Marchando en linea recta hacia la posicion fijada */
    ZOMBIE_STATE_RESPAWN_WAIT  /* Esperando temporizador para reaparecer dinamicamente */
} ZombieState;

/* Carga la textura del zombie (ZOMBIE.TIM) en VRAM */
void zombie_init(void);

/* Configura un enemigo con las propiedades iniciales del zombie */
void zombie_setup(Enemy *enemy, int32_t x, int32_t y);

/* Actualiza la lógica, movimiento, detección de obstáculos y reaparición del zombie */
void zombie_update(Enemy *enemy, int32_t many_x, int32_t many_y, int32_t level_w, int32_t level_h);

/* Renderiza el zombie aplicando la posición de cámara y orientación */
void zombie_render(Enemy *enemy, int32_t cam_x, int32_t cam_y);

/* Aplica daño al zombie según las reglas de SotN (invulnerable mientras emerge) */
uint8_t zombie_take_damage(Enemy *enemy, uint16_t damage);
