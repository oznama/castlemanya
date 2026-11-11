#include "../helpers/utils.h"

typedef enum
{
    MANY_STATE_IDLE,
    MANY_STATE_RUN,
    MANY_STATE_ATTACK,
    MANY_STATE_JUMP,
    MANY_STATE_FALL
} ManyState;

typedef struct
{
    uint32_t x, y;
    uint8_t direction;
    uint8_t grouded;
    uint8_t has_double_jump; // Si ya obtuvo el powerup
    uint8_t can_double_jump; // Disponible para usarse en el aire actual
    unsigned char r, g, b;
    ManyState current_state;
    SPRITE_OBJECT anim[10];
} MANY_OBJECT;

void many_init(void);
MANY_OBJECT many_render(uint16_t held_buttons, uint16_t pressed_buttons);