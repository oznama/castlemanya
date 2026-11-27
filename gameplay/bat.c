#include "bat.h"
#include <stdio.h>
#include <stdlib.h>
#include "../engine/cd_loader.h"
#include "../graphics/graphic.h"
#include "../graphics/animation.h"

#define BAT_SPRITE_SIZE 16
#define BAT_TRIGGER_DIST_X 120
#define BAT_TRIGGER_DIST_Y 140
#define BAT_SPEED 2

static CD_TEXTURE s_bat_tim;
static uint8_t s_bat_tim_loaded = 0;

void bat_init(void)
{
    if (!s_bat_tim_loaded)
    {
        if (cd_load_image("\\BAT.TIM", &s_bat_tim) == 0)
        {
            s_bat_tim_loaded = 1;
        }
        else
        {
            printf("Error cargando BAT.TIM\n");
        }
    }
}

void bat_setup(Enemy *enemy, int32_t x, int32_t y)
{
    if (!enemy)
        return;

    enemy->type = ENEMY_BAT;
    enemy->active = 1;
    enemy->state = BAT_STATE_HANGING;
    enemy->x = x;
    enemy->y = y;
    enemy->initial_x = x;
    enemy->initial_y = y;
    enemy->velocity_x = 0;
    enemy->velocity_y = 0;
    enemy->direction = 0; /* Por defecto mirando a la izquierda */

    enemy->hp = 1;
    enemy->max_hp = 1;
    enemy->damage = 5;
    enemy->exp = 10;
    enemy->invulnerable_timer = 0;

    enemy->col_w = BAT_SPRITE_SIZE;
    enemy->col_h = BAT_SPRITE_SIZE;

    /* Configuración de animación para reposo */
    enemy->anim.w = BAT_SPRITE_SIZE;
    enemy->anim.h = BAT_SPRITE_SIZE;
    enemy->anim.u = 0;
    enemy->anim.v = 0;
    enemy->anim.current_frame = 0;
    enemy->anim.total_frames = 1;
    enemy->anim.anim_timer = 0;
    enemy->anim.anim_speed = 1;
    enemy->anim.anim_dir = 1;
}

void bat_update(Enemy *enemy, int32_t many_x, int32_t many_y, int32_t level_w, int32_t level_h)
{
    if (!enemy || !enemy->active)
        return;

    (void)level_h;

    switch (enemy->state)
    {
    case BAT_STATE_HANGING:
    {
        /* Mientras Many no se acerque, se mantiene en reposo pegado al techo */
        int32_t dist_x = many_x - enemy->x;
        if (dist_x < 0)
            dist_x = -dist_x;

        int32_t diff_y = many_y - enemy->y;

        /* Si Many se encuentra cerca horizontalmente y está debajo o a su nivel */
        if (dist_x <= BAT_TRIGGER_DIST_X && diff_y >= -16 && diff_y <= BAT_TRIGGER_DIST_Y)
        {
            /* Pasar de reposo a despertar (transición a vuelo) */
            enemy->state = BAT_STATE_WAKING;
            enemy->direction = (many_x >= enemy->x) ? 1 : 0;

            /* Guardar la altura fija del objetivo al momento de la detección (no cambia si Many salta) */
            enemy->velocity_y = (int16_t)(many_y + 8);

            /* Frames de transición: 16,0 a 48,0 (3 frames: 16, 32, 48) */
            enemy->anim.u = 16;
            enemy->anim.v = 0;
            enemy->anim.current_frame = 0;
            enemy->anim.total_frames = 3;
            enemy->anim.anim_timer = 0;
            enemy->anim.anim_speed = 6;
            enemy->anim.anim_dir = 1;
        }
        break;
    }

    case BAT_STATE_WAKING:
    {
        /* Reproducir los 3 frames de transición de reposo a vuelo */
        enemy->anim.anim_timer++;
        if (enemy->anim.anim_timer >= enemy->anim.anim_speed)
        {
            enemy->anim.anim_timer = 0;
            enemy->anim.current_frame++;
            if (enemy->anim.current_frame >= enemy->anim.total_frames)
            {
                /* Transición completada: pasar a vuelo */
                enemy->state = BAT_STATE_FLYING;
                enemy->velocity_x = enemy->direction ? BAT_SPEED : -BAT_SPEED;

                /* Animación de vuelo: fila 1 (0,16 a 48,16).
                 * total_frames = 4 (frames 0, 1, 2, 3 correspondientes a u = 0, 16, 32, 48).
                 * Con anim_dir y update_animation(), la animación oscilará alternada:
                 * 0 -> 1 -> 2 -> 3 -> 2 -> 1 -> 0 para que el aleteo sea fluido y continuo.
                 */
                enemy->anim.u = 0;
                enemy->anim.v = 16;
                enemy->anim.current_frame = 0;
                enemy->anim.total_frames = 4;
                enemy->anim.anim_timer = 0;
                enemy->anim.anim_speed = 4;
                enemy->anim.anim_dir = 1;
            }
        }
        break;
    }

    case BAT_STATE_FLYING:
    {
        /* Movimiento horizontal continuo en la dirección fijada */
        enemy->x += enemy->velocity_x;

        /* Descenso/ascenso continuo y fluido hacia la altura fijada en la detección */
        int32_t target_y = (int32_t)enemy->velocity_y;
        if (enemy->y < target_y)
        {
            enemy->y += 1;
            if (enemy->y > target_y)
                enemy->y = target_y;
        }
        else if (enemy->y > target_y)
        {
            enemy->y -= 1;
            if (enemy->y < target_y)
                enemy->y = target_y;
        }

        /* Actualizar animación con aleteo alternado (ida y vuelta continuo) */
        update_animation(&enemy->anim);

        /* Si sale del nivel horizontalmente, se desactiva */
        if (enemy->x < -BAT_SPRITE_SIZE || enemy->x > level_w + BAT_SPRITE_SIZE)
        {
            enemy->active = 0;
        }
        break;
    }

    default:
        break;
    }
}

void bat_render(Enemy *enemy, int32_t cam_x, int32_t cam_y)
{
    if (!enemy || !enemy->active || !s_bat_tim_loaded)
        return;

    int32_t screen_x = enemy->x - cam_x;
    int32_t screen_y = enemy->y - cam_y;

    /* Culling de pantalla: solo renderizar si es visible */
    if (screen_x < SCREEN_XRES &&
        screen_x + BAT_SPRITE_SIZE > 0 &&
        screen_y < SCREEN_YRES &&
        screen_y + BAT_SPRITE_SIZE > 0)
    {
        /* Parpadeo visual si recibió daño */
        uint8_t r = 128, g = 128, b = 128;
        if (enemy->invulnerable_timer > 0 && (enemy->invulnerable_timer & 2))
        {
            r = 255;
            g = 64;
            b = 64;
        }

        uint8_t u = (uint8_t)(enemy->anim.u + enemy->anim.current_frame * BAT_SPRITE_SIZE);
        uint8_t v = (uint8_t)enemy->anim.v;

        /* Invertir horizontalmente según la dirección:
         * El sprite base mira hacia la izquierda. Si direction == 1 (derecha), volteamos (rotateu = 1).
         */
        render_poly_ft4(
            s_bat_tim,
            screen_x,
            screen_y,
            BAT_SPRITE_SIZE,
            BAT_SPRITE_SIZE,
            u,
            v,
            r, g, b,
            enemy->direction ? 1 : 0, 0);
    }
}
