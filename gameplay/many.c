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
#define MANY_SIZE 64

int32_t y_velocity;
int32_t x_velocity;

MANY_OBJECT many = {
    .current_state = MANY_STATE_IDLE,                         // Estado base respirando
    .x = 10,                                                  // Posicion en X
    .y = (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE, // Posicion en Y
    .direction = 1,                                           // Direccion derecha
    .grouded = 1,                                             // En el piso
    .has_double_jump = 0,
    .can_double_jump = 0,
    .r = 128, // Color R
    .g = 128, // Color G
    .b = 128, // Color B
    .anim = {
        {32, 64, 0, 0, 0, 4, 0, 20}}};

void many_change_state(ManyState new_state);
uint8_t many_is_state_locked(void);
void many_move_hor(uint16_t held_buttons);
void many_move_ver(uint16_t pressed_buttons, uint16_t held_buttons);
void many_action_attack(uint16_t pressed_buttons);

CD_TEXTURE many_tim;

void many_init(void)
{
    // Load logo image
    cd_load_image("\\MANY.TIM", &many_tim);
}

MANY_OBJECT many_render(uint16_t held_buttons, uint16_t pressed_buttons)
{
    // 1. Si no esta bloqueado por una animacion prioritaria (ej. ataque), procesamos inputs
    if (!many_is_state_locked())
    {
        many_move_hor(held_buttons);
        // many_action_attack(pressed_buttons);
    }

    // Procesamos fisica y salto
    // many_move_ver(pressed_buttons, held_buttons);

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
    // if (anim_finished)
    // {
    //     if (many.current_state == MANY_STATE_ATTACK)
    //     {
    //         if (many.grouded)
    //             many_change_state(MANY_STATE_IDLE);
    //         else
    //             many_change_state(y_velocity > 0 ? MANY_STATE_FALL : MANY_STATE_JUMP);
    //     }
    // }
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

void many_move_hor(uint16_t held_buttons)
{
    // Movimientos correr de many
    if ((held_buttons & PAD_LEFT))
    {
        if (many.x > 0)
            many.x -= MOVE_SPEED;
        many.direction = 0;
        if (many.grouded)
            many_change_state(MANY_STATE_RUN);
    }
    else if ((held_buttons & PAD_RIGHT))
    {
        if (many.x < (SCREEN_XRES - (MANY_SIZE + MANY_SIZE / 2)))
            many.x += MOVE_SPEED;
        many.direction = 1;
        if (many.grouded)
            many_change_state(MANY_STATE_RUN);
    }
    else
    {
        if (many.grouded)
            many_change_state(MANY_STATE_IDLE);
    }
}

void many_move_ver(uint16_t pressed_buttons, uint16_t held_buttons)
{
    // Si presiona Cruz y no está atacando
    if (!many_is_state_locked() && (pressed_buttons & PAD_CROSS))
    {

        // CASO A: Salto desde el piso
        if (many.grouded)
        {
            y_velocity = -JUMP_SPEED;
            many.grouded = 0;
            many.can_double_jump = many.has_double_jump; // Habilita el doble salto en el aire
            many_change_state(MANY_STATE_JUMP);

            // Inercia inicial al despegar
            if (held_buttons & PAD_LEFT)
                x_velocity = -MOVE_SPEED;
            else if (held_buttons & PAD_RIGHT)
                x_velocity = MOVE_SPEED;
            else
                x_velocity = 0;
        }
        // CASO B: Doble salto en el aire (estilo SotN)
        else if (many.can_double_jump)
        {
            y_velocity = -JUMP_SPEED;           // Impulso vertical completo
            many.can_double_jump = 0;           // Se consume el salto
            many_change_state(MANY_STATE_JUMP); // Reinicia animación de salto

            // En SotN puedes redirigir la inercia en el aire con el doble salto:
            if (held_buttons & PAD_LEFT)
            {
                x_velocity = -MOVE_SPEED;
                many.direction = 0;
            }
            else if (held_buttons & PAD_RIGHT)
            {
                x_velocity = MOVE_SPEED;
                many.direction = 1;
            }
            // Si no presiona flechas, conserva la inercia que traía
        }
    }

    if (!many.grouded)
    {
        // Aplicar movimiento horizontal por inercia
        many.x += x_velocity;

        // Limites de pantalla horizontales
        if ((int32_t)many.x < 0)
        {
            many.x = 0;
            x_velocity = 0;
        }
        else if (many.x > SCREEN_XRES - (MANY_SIZE + MANY_SIZE / 2))
        {
            many.x = SCREEN_XRES - (MANY_SIZE + MANY_SIZE / 2);
            x_velocity = 0;
        }

        // Movimiento vertical y gravedad
        many.y += y_velocity;
        y_velocity += GRAVITY;

        // Transicion a caida si va hacia abajo y no esta atacando
        if (y_velocity > 0 && many.current_state != MANY_STATE_ATTACK)
        {
            many_change_state(MANY_STATE_FALL);
        }

        // Aterrizaje en el suelo
        if (many.y >= (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE)
        {
            many.y = (SCREEN_YRES - MANY_SIZE - FLOOR_LIMIT) * FIXED_ONE;
            y_velocity = 0;
            x_velocity = 0;
            many.grouded = 1;
            many.can_double_jump = many.has_double_jump; // Recargar al tocar tierra

            // Al tocar el suelo, volver a IDLE o RUN si ya no está atacando
            if (many.current_state != MANY_STATE_ATTACK)
            {
                if ((held_buttons & PAD_LEFT) || (held_buttons & PAD_RIGHT))
                {
                    many_change_state(MANY_STATE_RUN);
                }
                else
                {
                    many_change_state(MANY_STATE_IDLE);
                }
            }
        }
    }
}

void many_action_attack(uint16_t pressed_buttons)
{
    if ((pressed_buttons & PAD_SQUARE))
        many_change_state(MANY_STATE_ATTACK);
}