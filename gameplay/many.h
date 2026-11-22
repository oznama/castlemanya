#include "../graphics/animation.h"

#define MANY_COLLISION_WIDTH 16
#define MANY_COLLISION_HEIGHT 64

typedef enum
{
    MANY_STATE_IDLE,
    MANY_STATE_RUN,
    MANY_STATE_ATTACK,
    MANY_STATE_JUMP,
    MANY_STATE_FALL,
    MANY_STATE_CROUNCH,
    MANY_STATE_HURT,
    MANY_STATE_BACKDASH,
    MANY_STATE_HIGHJUMP
} ManyState;

typedef enum
{
    MANY_WEAPON_WHIP,
    MANY_WEAPON_SPEAR,
    MANY_WEAPON_CROSS_AXE,
    MANY_WEAPON_HOLY_WATER
} ManyWeaponType;

typedef struct
{
    int hp, mp;
    uint32_t x, y;
    uint8_t direction;
    uint8_t grouded;
    uint8_t has_double_jump; // Si ya obtuvo el powerup
    uint8_t can_double_jump; // Disponible para usarse en el aire actual
    uint8_t has_high_jump; // Disponible si ya obtuvo dicha reliquia
    unsigned char r, g, b;
    ManyState current_state;
    ManyWeaponType weapon;
    SPRITE_OBJECT anim[10];
} MANY_OBJECT;

void many_init(void);
MANY_OBJECT* many_get_instance(void);
void many_set_position(int32_t world_x, int32_t world_y);
void many_render_in_camera(int32_t cam_x, int32_t cam_y);
void many_change_state(ManyState new_state);
void many_auto_walk_step(int8_t direction);

/* Actualiza y resuelve el movimiento y colisiones de Many en el nivel dinamico */
void many_update_in_level(uint16_t held_buttons, uint16_t pressed_buttons, uint16_t buttons_history[10], void *dynamic_level_ptr);