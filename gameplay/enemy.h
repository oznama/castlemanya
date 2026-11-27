#pragma once

#include <stdint.h>
#include "../graphics/sprite.h"

/* Tipos de enemigos soportados en el juego */
typedef enum
{
    ENEMY_NONE = 0,
    ENEMY_BAT,
    ENEMY_ZOMBIE,
    ENEMY_SKELETON,
    ENEMY_MEDUSA
} EnemyType;

/* Estructura genérica para todos los enemigos del juego */
typedef struct Enemy
{
    // Identificación y tipo
    EnemyType type;
    uint8_t active;
    uint8_t state; // Estado interno del enemigo específico

    // Posición y física
    int32_t x;          // Coordenada X en espacio de mundo
    int32_t y;          // Coordenada Y en espacio de mundo
    int32_t initial_x;  // Posición X inicial para reaparición
    int32_t initial_y;  // Posición Y inicial para reaparición
    int16_t velocity_x; // Velocidad horizontal
    int16_t velocity_y; // Velocidad vertical
    uint8_t direction;  // 0: Izquierda, 1: Derecha

    // Atributos de combate y RPG
    int16_t hp;                 // Puntos de vida actuales
    int16_t max_hp;             // Vida máxima
    uint16_t damage;            // Daño infligido al jugador Many
    uint16_t exp;               // Experiencia otorgada al ser derrotado
    uint8_t invulnerable_timer; // Temporizador de invulnerabilidad / parpadeo de daño

    // Hitbox y animación
    int16_t col_w;      // Ancho de la caja de colisión
    int16_t col_h;      // Alto de la caja de colisión
    SPRITE_OBJECT anim; // Manejo de frames, timers y velocidad de animación
} Enemy;

/* Inicializa el sistema global de enemigos */
void enemy_system_init(void);

/* Inicializa un enemigo según su tipo con valores predeterminados */
void enemy_init(Enemy *enemy, EnemyType type, int32_t x, int32_t y);

/* Restablece un enemigo a su estado y posición original al recargar la habitación */
void enemy_reset(Enemy *enemy);

/* Actualiza la lógica, movimiento e IA del enemigo */
void enemy_update(Enemy *enemy, int32_t many_x, int32_t many_y, int32_t level_w, int32_t level_h);

/* Renderiza el enemigo en pantalla aplicando el offset de la cámara */
void enemy_render(Enemy *enemy, int32_t cam_x, int32_t cam_y);

/* Aplica daño a un enemigo. Retorna 1 si el enemigo murió con este golpe */
uint8_t enemy_take_damage(Enemy *enemy, uint16_t damage);
