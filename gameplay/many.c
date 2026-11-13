#include <psxgpu.h>
#include <psxpad.h>

#include "many.h"
#include "../engine/cd_image.h"
#include "../graphics/graphic.h"

#define MOVE_SPEED 2
#define FIXED_SHIFT 8
#define FIXED_ONE (1 << FIXED_SHIFT)
#define JUMP_SPEED (8 * FIXED_ONE)
#define GRAVITY (FIXED_ONE / 2)

/// Sugerencia de la IA

#define LEVEL_MAP_COLS 20
#define LEVEL_MAP_ROWS 15
#define TILE_SIZE 16

#define MANY_W 32
#define MANY_H 64

MANY_OBJECT many = {
    .current_state = MANY_STATE_IDLE, // Estado base respirando
    .x = 10,                          // Posicion en X
    // .y = (SCREEN_YRES - MANY_H - FLOOR_LIMIT) * FIXED_ONE, // Posicion en Y
    .y = (128 - MANY_H) * FIXED_ONE,
    .direction = 1, // Direccion derecha
    .grouded = 1,                                             // En el piso
    .has_double_jump = 1,
    .can_double_jump = 0,
    .r = 128, // Color R
    .g = 128, // Color G
    .b = 128, // Color B
    .anim = {
        {64, 64, 0, 0, 0, 4, 0, 20},
        {64, 64, 0, 64, 0, 3, 0, 16},
        {64, 64, 0, 128, 0, 4, 0, 8},
        {64, 64, 0, 192, 0, 1, 0, 6},
        {64, 64, 64, 192, 0, 1, 0, 6},
        {64, 64, 128, 192, 0, 1, 0, 6}}};

static uint8_t map_is_solid_at(const short level_map[], int x, int y)
{
    if (x < 0 || y < 0 ||
        x >= LEVEL_MAP_COLS * TILE_SIZE ||
        y >= LEVEL_MAP_ROWS * TILE_SIZE)
    {
        return 1;
    }

    return level_map[(y / TILE_SIZE) * LEVEL_MAP_COLS +
                     (x / TILE_SIZE)] != 0;
}

static uint8_t map_is_solid_at_fixed_y(
    const short level_map[], int x, int32_t y_fixed)
{
    if (y_fixed < 0)
    {
        return 1;
    }

    return map_is_solid_at(level_map, x, (int)(y_fixed / FIXED_ONE));
}

static uint8_t many_hits_side(
    const short level_map[], int next_x, int direction)
{
    int edge_x = direction < 0 ? next_x : next_x + MANY_W - 1;

    return map_is_solid_at_fixed_y(level_map, edge_x,
                                   many.y + 4 * FIXED_ONE) ||
           map_is_solid_at_fixed_y(level_map, edge_x,
                                   many.y + (MANY_H / 2) * FIXED_ONE) ||
           map_is_solid_at_fixed_y(level_map, edge_x,
                                   many.y + (MANY_H - 4) * FIXED_ONE);
}

static uint8_t many_has_floor(const short level_map[])
{
    int32_t foot_y = many.y + MANY_H * FIXED_ONE;

    return map_is_solid_at_fixed_y(level_map, many.x + 1, foot_y) ||
           map_is_solid_at_fixed_y(level_map, many.x + MANY_W - 2, foot_y);
}

///////

void many_change_state(ManyState new_state);
uint8_t many_is_state_locked(void);
void many_move_hor(uint16_t held_buttons, const short level_map[]);
void many_move_ver(uint16_t pressed_buttons, uint16_t held_buttons, const short level_map[]);
void many_action_attack(uint16_t pressed_buttons);
void many_move_crouch(uint16_t held_buttons);

CD_TEXTURE many_tim;

void many_init(void)
{
    // Load logo image
    cd_load_image("\\MANY.TIM", &many_tim);
}

MANY_OBJECT many_render(uint16_t held_buttons, uint16_t pressed_buttons, const short level_map[])
{
    // 1. Si no esta bloqueado por una animacion prioritaria (ej. ataque), procesamos inputs
    if (!many_is_state_locked())
    {
        many_move_hor(held_buttons, level_map);
        many_action_attack(pressed_buttons);
        many_move_crouch(held_buttons);
    }

    // Procesamos fisica y salto
    many_move_ver(pressed_buttons, held_buttons, level_map);

    // Rendereando a many en modo sprite simple (sin rotacion)
    // render_sprt(many_tim,
    //     many.x + 100, (many.y >> FIXED_SHIFT),
    //     many.anim[many_current_frame].w, many.anim[many_current_frame].h,
    //     many.anim[many_current_frame].u, many.anim[many_current_frame].v,
    //     many.r, many.g, many.b
    // );

    // 2. Actualizamos la animacion y verificamos si termino
    uint8_t anim_finished = update_animation(&many.anim[many.current_state]);

    // 3. Si termino una animacion que mantenia ocupado al personaje, volvemos a IDLE
    if (anim_finished)
    {
        if (many.current_state == MANY_STATE_ATTACK)
        {
            if (many.grouded)
                many_change_state(MANY_STATE_IDLE);
            else
                many_change_state(many.y_velocity > 0 ? MANY_STATE_FALL : MANY_STATE_JUMP);
        }
    }
    // 4. Rendereando a many como poligono de 4 vertices (con rotacion)
    render_poly_ft4(many_tim,
                    many.x, (many.y >> FIXED_SHIFT),
                    many.anim[many.current_state].w, many.anim[many.current_state].h,
                    many.anim[many.current_state].u + (many.anim[many.current_state].current_frame * many.anim[many.current_state].w), many.anim[many.current_state].v,
                    many.r, many.g, many.b,
                    !many.direction, 0);

    return many;
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

void many_move_hor(uint16_t held_buttons, const short level_map[])
{
    int dx = 0;
    uint8_t moved = 0;

    if (many.grouded)
    {
        if (held_buttons & PAD_LEFT)
        {
            dx = -MOVE_SPEED;
            many.direction = 0;
        }
        else if (held_buttons & PAD_RIGHT)
        {
            dx = MOVE_SPEED;
            many.direction = 1;
        }
    }
    else
    {
        dx = many.x_velocity;
    }

    if (dx != 0)
    {
        int next_x = many.x + dx;

        if (!many_hits_side(level_map, next_x, dx))
        {
            many.x = next_x;
            moved = 1;
        }
        else if (!many.grouded)
        {
            many.x_velocity = 0;
        }
    }

    if (many.grouded)
    {
        many_change_state(moved ? MANY_STATE_RUN : MANY_STATE_IDLE);
    }
}

void many_move_ver(
    uint16_t pressed_buttons,
    uint16_t held_buttons,
    const short level_map[])
{
    if (!many_is_state_locked() && (pressed_buttons & PAD_CROSS))
    {
        if (many.grouded)
        {
            many.y_velocity = -JUMP_SPEED;
            many.grouded = 0;
            many.can_double_jump = many.has_double_jump;
            many_change_state(MANY_STATE_JUMP);

            if (held_buttons & PAD_LEFT)
            {
                many.x_velocity = -MOVE_SPEED;
            }
            else if (held_buttons & PAD_RIGHT)
            {
                many.x_velocity = MOVE_SPEED;
            }
            else
            {
                many.x_velocity = 0;
            }
        }
        else if (many.can_double_jump)
        {
            many.y_velocity = -JUMP_SPEED;
            many.can_double_jump = 0;
            many_change_state(MANY_STATE_JUMP);

            if (held_buttons & PAD_LEFT)
            {
                many.x_velocity = -MOVE_SPEED;
                many.direction = 0;
            }
            else if (held_buttons & PAD_RIGHT)
            {
                many.x_velocity = MOVE_SPEED;
                many.direction = 1;
            }
        }
    }

    if (many.grouded && !many_has_floor(level_map))
    {
        many.grouded = 0;

        if (many.current_state != MANY_STATE_ATTACK)
        {
            many_change_state(MANY_STATE_FALL);
        }
    }

    if (!many.grouded)
    {
        int32_t next_y = many.y + many.y_velocity;

        if (many.y_velocity > 0)
        {
            int32_t foot_y = next_y + MANY_H * FIXED_ONE - 1;

            if (map_is_solid_at_fixed_y(level_map, many.x + 1, foot_y) ||
                map_is_solid_at_fixed_y(
                    level_map, many.x + MANY_W - 2, foot_y))
            {
                int foot_pixel = foot_y < 0 ? 0 : (int)(foot_y / FIXED_ONE);
                int tile_row = foot_pixel / TILE_SIZE;

                many.y = (tile_row * TILE_SIZE - MANY_H) * FIXED_ONE;
                many.y_velocity = 0;
                many.x_velocity = 0;
                many.grouded = 1;
                many.can_double_jump = many.has_double_jump;

                if (many.current_state != MANY_STATE_ATTACK)
                {
                    if (held_buttons & (PAD_LEFT | PAD_RIGHT))
                    {
                        many_change_state(MANY_STATE_RUN);
                    }
                    else
                    {
                        many_change_state(MANY_STATE_IDLE);
                    }
                }
            }
            else
            {
                many.y = next_y;
                many.y_velocity += GRAVITY;
            }
        }
        else if (many.y_velocity < 0)
        {
            if (map_is_solid_at_fixed_y(level_map, many.x + 1, next_y) ||
                map_is_solid_at_fixed_y(
                    level_map, many.x + MANY_W - 2, next_y))
            {
                int head_pixel = next_y < 0 ? 0 : (int)(next_y / FIXED_ONE);
                int tile_row = head_pixel / TILE_SIZE;

                many.y = ((tile_row + 1) * TILE_SIZE) * FIXED_ONE;
                many.y_velocity = 0;
            }
            else
            {
                many.y = next_y;
                many.y_velocity += GRAVITY;
            }
        }
        else
        {
            many.y = next_y;
            many.y_velocity += GRAVITY;
        }

        if (!many.grouded &&
            many.y_velocity > 0 &&
            many.current_state != MANY_STATE_ATTACK)
        {
            many_change_state(MANY_STATE_FALL);
        }
    }
}


void many_action_attack(uint16_t pressed_buttons)
{
    if ((pressed_buttons & PAD_SQUARE))
        many_change_state(MANY_STATE_ATTACK);
}

void many_move_crouch(uint16_t held_buttons)
{
    if ((held_buttons & PAD_DOWN))
        many_change_state(MANY_STATE_CROUNCH);
}