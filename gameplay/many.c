#include <psxgpu.h>
#include <psxpad.h>

#include "many.h"
#include "../engine/cd_image.h"
#include "../graphics/graphic.h"
#include "../helpers/level_generator.h"

#define MOVE_SPEED 2
#define FIXED_SHIFT 8
#define FIXED_ONE (1 << FIXED_SHIFT)
#define JUMP_SPEED (8 * FIXED_ONE)
#define GRAVITY (FIXED_ONE / 2)
#define MANY_SIZE MANY_COLLISION_HEIGHT
#define BACKDASH_DISTANCE (2 * MANY_COLLISION_WIDTH)
#define BACKDASH_SPEED 6
#define HIGHJUMP_SPEED ((196 * FIXED_ONE) / 10)
#define HIGHJUMP_INPUT_WINDOW 20

static int32_t backdash_remaining;
static int8_t backdash_direction;
static uint8_t highjump_input_timer;

int32_t y_velocity;
int32_t x_velocity;

MANY_OBJECT many = {
    .current_state = MANY_STATE_IDLE,                         // Estado base respirando
    .x = 10,                                                  // Posicion en X
    .y = (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE, // Posicion en Y
    .direction = 1,                                           // Direccion derecha
    .grouded = 1,                                             // En el piso
    .has_double_jump = 1,
    .can_double_jump = 0,
    .r = 128, // Color R
    .g = 128, // Color G
    .b = 128, // Color B
    .anim = {
        [MANY_STATE_IDLE] = {
            .w = 32, .h = 64, .u = 0, .v = 0, .current_frame = 0, .total_frames = 4, .anim_timer = 0, .anim_speed = 16, .anim_dir = 1
        },
        [MANY_STATE_RUN] = {
            .w = 48, .h = 48, .u = 0, .v = 0, .current_frame = 0, .total_frames = 3, .anim_timer = 0, .anim_speed = 16, .anim_dir = 1
        },
        [MANY_STATE_ATTACK] = {
            .w = 64, .h = 64, .u = 0, .v = 0, .current_frame = 0, .total_frames = 4, .anim_timer = 0, .anim_speed = 8, .anim_dir = 1
        },
        [MANY_STATE_JUMP] = {
            .w = 32, .h = 64, .u = 0, .v = 64, .current_frame = 0, .total_frames = 1, .anim_timer = 0, .anim_speed = 1, .anim_dir = 1
        },
        [MANY_STATE_FALL] = {
            .w = 32, .h = 64, .u = 32, .v = 64, .current_frame = 0, .total_frames = 1, .anim_timer = 0, .anim_speed = 1, .anim_dir = 1
        },
        [MANY_STATE_CROUNCH] = {
            .w = 32, .h = 64, .u = 64, .v = 64, .current_frame = 0, .total_frames = 1, .anim_timer = 0, .anim_speed = 1, .anim_dir = 1
        },
        [MANY_STATE_HURT] = {
            .w = 32, .h = 64, .u = 96, .v = 64, .current_frame = 0, .total_frames = 1, .anim_timer = 0, .anim_speed = 1, .anim_dir = 1
        },
        [MANY_STATE_BACKDASH] = {
            .w = 32, .h = 64, .u = 0, .v = 128, .current_frame = 0, .total_frames = 1, .anim_timer = 0, .anim_speed = 1, .anim_dir = 1
        },
        [MANY_STATE_HIGHJUMP] = {
            .w = 32, .h = 64, .u = 32, .v = 128, .current_frame = 0, .total_frames = 1, .anim_timer = 0, .anim_speed = 1, .anim_dir = 1
        }
    }
};

void many_change_state(ManyState new_state);
uint8_t many_is_state_locked(void);
void many_action_attack(uint16_t pressed_buttons);
void many_move_crouch(uint16_t held_buttons);

CD_TEXTURE many_base_tim;
CD_TEXTURE many_action_tim;
CD_TEXTURE many_attack_tim;

static CD_TEXTURE *many_texture_for_state(ManyState state)
{
    switch (state)
    {
    case MANY_STATE_RUN:
        return &many_action_tim;
    case MANY_STATE_ATTACK:
        return &many_attack_tim;
    default:
        return &many_base_tim;
    }
}

static void many_draw_sprite(int32_t x, int32_t y)
{
    SPRITE_OBJECT *anim = &many.anim[many.current_state];
    int32_t draw_x = x + (MANY_COLLISION_WIDTH - (int32_t)anim->w) / 2;
    int32_t draw_y = y + (MANY_COLLISION_HEIGHT - (int32_t)anim->h);

    render_poly_ft4(*many_texture_for_state(many.current_state),
                    draw_x, draw_y,
                    anim->w, anim->h,
                    (uint8_t)(anim->u + anim->current_frame * anim->w),
                    (uint8_t)anim->v,
                    many.r, many.g, many.b,
                    !many.direction, 0);
}

void many_init(void)
{
    cd_load_image("\\MANYBAS.TIM", &many_base_tim);
    cd_load_image("\\MANYACT.TIM", &many_action_tim);
    cd_load_image("\\MANYATK.TIM", &many_attack_tim);
}

void many_change_state(ManyState new_state)
{
    if (many.current_state == new_state)
    {
        return;
    }

    many.current_state = new_state;

    // Reiniciar los valores de la animación que va a comenzar
    many.anim[new_state].current_frame = 0;
    many.anim[new_state].anim_timer = 0;
    many.anim[new_state].anim_dir = 1;
}

uint8_t many_is_state_locked(void)
{
    // Si Many está atacando, no permitimos cambiar de estado
    if (many.current_state == MANY_STATE_ATTACK)
    {
        return 1;
    }
    return 0;
}

void many_action_attack(uint16_t pressed_buttons)
{
    if ((pressed_buttons & PAD_SQUARE))
        many_change_state(MANY_STATE_ATTACK);
}

void many_move_crouch(uint16_t held_buttons)
{
    if (!many.grouded)
        return;

    if (held_buttons & PAD_DOWN)
    {
        many_change_state(MANY_STATE_CROUNCH);
    }
    else if (many.current_state == MANY_STATE_CROUNCH)
    {
        if (held_buttons & (PAD_LEFT | PAD_RIGHT))
            many_change_state(MANY_STATE_RUN);
        else
            many_change_state(MANY_STATE_IDLE);
    }
}

MANY_OBJECT *many_get_instance(void)
{
    return &many;
}

void many_set_position(int32_t world_x, int32_t world_y)
{
    many.x = world_x;
    many.y = world_y * FIXED_ONE;
}

void many_render_in_camera(int32_t cam_x, int32_t cam_y)
{
    int32_t screen_x = (int32_t)many.x - cam_x;
    int32_t screen_y = (int32_t)(many.y >> FIXED_SHIFT) - cam_y;

    many_draw_sprite(screen_x, screen_y);
}

void many_update_in_level(uint16_t held_buttons, uint16_t pressed_buttons, void *dynamic_level_ptr)
{
    DynamicLevel *level = (DynamicLevel *)dynamic_level_ptr;
    if (!level)
        return;

    // 1. Acciones de ataque y agacharse si no esta bloqueado
    uint8_t highjump_requested = 0;

    if (!many_is_state_locked())
    {
        highjump_requested =
            highjump_input_timer > 0 &&
            (held_buttons & PAD_UP) &&
            (pressed_buttons & PAD_CROSS) &&
            many.grouded;

        if (highjump_requested || (pressed_buttons & PAD_CROSS))
        {
            highjump_input_timer = 0;
        }
        else if (pressed_buttons & PAD_DOWN)
        {
            highjump_input_timer = HIGHJUMP_INPUT_WINDOW;
        }
        else if (highjump_input_timer > 0)
        {
            highjump_input_timer--;
        }

        many_action_attack(pressed_buttons);

        if (!many_is_state_locked())
        {
            many_move_crouch(held_buttons);
        }

        if (!many_is_state_locked() &&
            !highjump_requested &&
            backdash_remaining == 0 &&
            (pressed_buttons & PAD_TRIANGLE))
        {
            backdash_direction = many.direction ? -1 : 1;
            backdash_remaining = BACKDASH_DISTANCE;
            many_change_state(MANY_STATE_BACKDASH);
        }
    }
    else
    {
        highjump_input_timer = 0;
    }

    // 2. Movimiento horizontal
    int32_t vx = 0;
    if (backdash_remaining > 0)
    {
        int32_t dash_step = backdash_remaining < BACKDASH_SPEED
            ? backdash_remaining
            : BACKDASH_SPEED;

        vx = backdash_direction * dash_step;
    }
    else if (!many_is_state_locked())
    {
        if (!many_is_state_locked())
        {
            if (held_buttons & PAD_LEFT)
            {
                vx = -MOVE_SPEED;
                many.direction = 0;
                if (many.grouded && many.current_state != MANY_STATE_CROUNCH)
                    many_change_state(MANY_STATE_RUN);
            }
            else if (held_buttons & PAD_RIGHT)
            {
                vx = MOVE_SPEED;
                many.direction = 1;
                if (many.grouded && many.current_state != MANY_STATE_CROUNCH)
                    many_change_state(MANY_STATE_RUN);
            }
            else
            {
                if (many.grouded && many.current_state != MANY_STATE_CROUNCH && many.current_state != MANY_STATE_ATTACK)
                    many_change_state(MANY_STATE_IDLE);
            }
        }
    }

    // 3. Salto
    if (!many_is_state_locked() && backdash_remaining == 0 && (pressed_buttons & PAD_CROSS))
    {
        if (highjump_requested)
        {
            y_velocity = -HIGHJUMP_SPEED;
            many.grouded = 0;
            many.can_double_jump = many.has_double_jump;
            many_change_state(MANY_STATE_HIGHJUMP);
            x_velocity = vx;
        }
        else if (many.grouded)
        {
            y_velocity = -JUMP_SPEED;
            many.grouded = 0;
            many.can_double_jump = many.has_double_jump;
            many_change_state(MANY_STATE_JUMP);
            x_velocity = vx;
        }
        else if (many.can_double_jump)
        {
            y_velocity = -JUMP_SPEED;
            many.can_double_jump = 0;
            many_change_state(MANY_STATE_JUMP);
            if (vx != 0)
                x_velocity = vx;
        }
    }
    

    // Si esta en el aire, aplicar inercia horizontal y gravedad
    if (!many.grouded)
    {
        if (vx != 0)
            x_velocity = vx; // Permitir control aereo estilo SotN
        vx = x_velocity;

        y_velocity += GRAVITY;
        if (y_velocity > 0 && many.current_state != MANY_STATE_ATTACK && backdash_remaining == 0)
            many_change_state(MANY_STATE_FALL);
    }

    int32_t vy = y_velocity >> FIXED_SHIFT;

    // 4. Resolucion de colisiones con el nivel
    int32_t current_x = (int32_t)many.x;
    int32_t current_y = (int32_t)(many.y >> FIXED_SHIFT);
    bool is_grounded = false;

    int32_t previous_x = current_x;

    /* Usamos un ancho de colision realista centrado en el sprite de 64x64 */
    level_resolve_entity_collision(level,
                                   &current_x, &current_y,
                                   &vx, &vy,
                                   MANY_COLLISION_WIDTH,
                                   MANY_COLLISION_HEIGHT,
                                   &is_grounded);

    if (backdash_remaining > 0)
    {
        int32_t moved_x = current_x - previous_x;

        if (moved_x < 0)
            moved_x = -moved_x;

        if (moved_x == 0 || moved_x >= backdash_remaining)
            backdash_remaining = 0;
        else
            backdash_remaining -= moved_x;

        if (backdash_remaining == 0)
            x_velocity = 0;
    }

    many.x = current_x;
    many.y = current_y * FIXED_ONE;

    if (is_grounded)
    {
        y_velocity = 0;
        x_velocity = 0;
        many.grouded = 1;
        many.can_double_jump = many.has_double_jump;

        if (many.current_state != MANY_STATE_ATTACK && many.current_state != MANY_STATE_CROUNCH && backdash_remaining == 0)
        {
            if ((held_buttons & PAD_LEFT) || (held_buttons & PAD_RIGHT))
                many_change_state(MANY_STATE_RUN);
            else
                many_change_state(MANY_STATE_IDLE);
        }
    }
    else
    {
        many.grouded = 0;
        if (vy == 0 && y_velocity < 0)
        {
            // Golpe contra techo
            y_velocity = 0;
        }
    }

    if (many.current_state == MANY_STATE_BACKDASH && backdash_remaining == 0 && !many.grouded)
    {
        many_change_state(y_velocity > 0 ? MANY_STATE_FALL : MANY_STATE_JUMP);
    }

    // 5. Animaciones
    uint8_t anim_finished = update_animation(&many.anim[many.current_state]);
    if (anim_finished && many.current_state == MANY_STATE_ATTACK)
    {
        if (many.grouded)
            many_change_state(MANY_STATE_IDLE);
        else
            many_change_state(y_velocity > 0 ? MANY_STATE_FALL : MANY_STATE_JUMP);
    }
}
