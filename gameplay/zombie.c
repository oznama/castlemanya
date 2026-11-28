#include "zombie.h"
#include <stdio.h>
#include <stdlib.h>
#include "../engine/cd_loader.h"
#include "../graphics/graphic.h"
#include "../graphics/animation.h"
#include "../helpers/level/level_manager.h"
#include "../helpers/level/level_generator.h"

/*
 * Características de Zombie (SotN):
 * - Marcha lenta e implacable
 * - Emerge de la tierra
 * - Invulnerable mientras emerge
 * - Hitbox activo al iniciar la marcha
 * - Si choca contra pared o cae al vacío, se destruye y reaparece dinámicamente cerca de Many
 */

#define ZOMBIE_WALK_SPEED 1
#define ZOMBIE_RESPAWN_DELAY_FRAMES 75 /* ~1.25 segundos para reaparecer */
#define ZOMBIE_ANIM_SPEED_EMERGE 7     /* Frames por paso de emergencia */
#define ZOMBIE_ANIM_SPEED_WALK 8       /* Frames por paso de caminata */

/* Coordenadas UV fijas de los 5 frames de emergencia según la sprite sheet (96x96):
 * (0,0), (24,0), (48,0), (72,0), (0,48)
 */
static const struct {
    uint8_t u;
    uint8_t v;
} s_emerge_uv[5] = {{0, 64}, {24, 64}, {48, 64}, {72, 64}, {0, 112}};

/* Coordenadas UV de los 3 frames de caminata:
 * (24,48), (48,48), (72,48)
 */
static const struct {
    uint8_t u;
    uint8_t v;
} s_walk_uv[3] = {{24, 112}, {48, 112}, {72, 112}};

static CD_TEXTURE s_zombie_tim;
static uint8_t s_zombie_tim_loaded = 0;

/* Generador congruencial lineal determinista para PS1 */
static uint32_t s_zombie_rand_seed = 12345;
static uint16_t zombie_rand(void)
{
    s_zombie_rand_seed = s_zombie_rand_seed * 1103515245 + 12345;
    return (uint16_t)((s_zombie_rand_seed >> 16) & 0x7FFF);
}

void zombie_init(void)
{
    if (!s_zombie_tim_loaded)
    {
        if (cd_load_image("\\ZOMBIE.TIM", &s_zombie_tim) == 0)
        {
            s_zombie_tim_loaded = 1;
        }
        else
        {
            printf("Error cargando ZOMBIE.TIM\n");
        }
    }
}

/*
 * Busca una superficie de suelo sólida o plataforma en la columna world_x
 * partiendo de start_y hacia abajo.
 * Retorna true si encontró suelo y guarda la coordenada Y superior del piso.
 */
static bool zombie_find_floor(const DynamicLevel *level, int32_t world_x, int32_t start_y, int32_t *out_floor_y)
{
    if (!level || !out_floor_y)
        return false;

    if (world_x < 0 || world_x >= (int32_t)level->pixel_width)
        return false;

    int32_t tx = world_x / TILE_SIZE;
    int32_t start_ty = start_y / TILE_SIZE;
    if (start_ty < 0)
        start_ty = 0;

    int32_t max_ty = level->height;

    for (int32_t ty = start_ty; ty < max_ty; ty++)
    {
        TileCollisionType coll = level_get_tile_collision(level, tx, ty);
        if (coll == TILE_COLL_SOLID || coll == TILE_COLL_ONEWAY)
        {
            *out_floor_y = ty * TILE_SIZE;
            return true;
        }
    }

    return false;
}

/*
 * Intenta reaparecer dinámicamente al zombie en las cercanías de Many
 * sobre un suelo válido.
 */
static void zombie_try_spawn(Enemy *enemy, int32_t many_x, int32_t many_y, const DynamicLevel *level)
{
    if (!enemy || !level)
        return;

    int32_t level_w = (int32_t)level->pixel_width;
    int32_t level_h = (int32_t)level->pixel_height;

    /* Posición X candidata: delante o detrás de Many en un rango de 60 a 140 px */
    int16_t offset_mag = 64 + (int16_t)(zombie_rand() % 80);
    int32_t cand_x_1 = many_x + offset_mag;
    int32_t cand_x_2 = many_x - offset_mag;

    /* Alternar según random */
    int32_t chosen_x = (zombie_rand() & 1) ? cand_x_1 : cand_x_2;

    /* Validar dentro de los límites del nivel con margen */
    if (chosen_x < 16)
        chosen_x = (cand_x_1 > 16) ? cand_x_1 : 24;
    else if (chosen_x > level_w - ZOMBIE_SPRITE_W - 16)
        chosen_x = (cand_x_2 < level_w - ZOMBIE_SPRITE_W - 16) ? cand_x_2 : level_w - ZOMBIE_SPRITE_W - 24;

    /* Buscar piso debajo de Many o un poco más arriba */
    int32_t start_y = many_y - 32;
    if (start_y < 0)
        start_y = 0;

    int32_t floor_y = 0;
    if (zombie_find_floor(level, chosen_x + (ZOMBIE_SPRITE_W / 2), start_y, &floor_y))
    {
        int32_t spawn_y = floor_y - ZOMBIE_SPRITE_H;
        if (spawn_y >= 0 && spawn_y + ZOMBIE_SPRITE_H <= level_h)
        {
            enemy->x = chosen_x;
            enemy->y = spawn_y;
            enemy->velocity_x = 0;
            enemy->velocity_y = 0;
            /* Mirar hacia la posición actual de Many */
            enemy->direction = (many_x >= enemy->x) ? 1 : 0;
            enemy->state = ZOMBIE_STATE_EMERGING;
            enemy->hp = enemy->max_hp;
            enemy->invulnerable_timer = 0;
            enemy->respawn_timer = 0;
            enemy->active = 1;

            /* Configurar animación de emergencia (frame 0: 0,0) */
            enemy->anim.w = ZOMBIE_SPRITE_W;
            enemy->anim.h = ZOMBIE_SPRITE_H;
            enemy->anim.u = s_emerge_uv[0].u;
            enemy->anim.v = s_emerge_uv[0].v;
            enemy->anim.current_frame = 0;
            enemy->anim.total_frames = 5;
            enemy->anim.anim_timer = 0;
            enemy->anim.anim_speed = ZOMBIE_ANIM_SPEED_EMERGE;
            enemy->anim.anim_dir = 1;
            return;
        }
    }

    /* Si no se pudo hallar piso en la posición elegida, reintentar en el próximo frame */
    enemy->respawn_timer = 10;
}

void zombie_setup(Enemy *enemy, int32_t x, int32_t y)
{
    if (!enemy)
        return;

    enemy->type = ENEMY_ZOMBIE;
    enemy->active = 1;
    enemy->state = ZOMBIE_STATE_EMERGING;
    enemy->x = x;
    enemy->y = y;
    enemy->initial_x = x;
    enemy->initial_y = y;
    enemy->velocity_x = 0;
    enemy->velocity_y = 0;
    enemy->direction = 0;

    enemy->hp = 2;
    enemy->max_hp = 2;
    enemy->damage = 6;
    enemy->exp = 12;
    enemy->invulnerable_timer = 0;
    enemy->respawn_timer = 0;

    enemy->col_w = ZOMBIE_SPRITE_W;
    enemy->col_h = ZOMBIE_SPRITE_H;

    /* Iniciar con la secuencia de emergencia (5 frames) */
    enemy->anim.w = ZOMBIE_SPRITE_W;
    enemy->anim.h = ZOMBIE_SPRITE_H;
    enemy->anim.u = s_emerge_uv[0].u;
    enemy->anim.v = s_emerge_uv[0].v;
    enemy->anim.current_frame = 0;
    enemy->anim.total_frames = 5;
    enemy->anim.anim_timer = 0;
    enemy->anim.anim_speed = ZOMBIE_ANIM_SPEED_EMERGE;
    enemy->anim.anim_dir = 1;
}

void zombie_update(Enemy *enemy, int32_t many_x, int32_t many_y, int32_t level_w, int32_t level_h)
{
    if (!enemy)
        return;

    /* Manejo del estado de espera para reaparición dinámica */
    if (enemy->state == ZOMBIE_STATE_RESPAWN_WAIT)
    {
        if (enemy->respawn_timer > 0)
        {
            enemy->respawn_timer--;
            return;
        }

        /* Tiempo cumplido: reaparecer cerca de Many */
        DynamicLevel *level = level_manager_get_current_level();
        zombie_try_spawn(enemy, many_x, many_y, level);
        return;
    }

    if (!enemy->active)
        return;

    DynamicLevel *level = level_manager_get_current_level();

    switch (enemy->state)
    {
    case ZOMBIE_STATE_EMERGING:
    {
        /* Al emerger: orientarse hacia la posición inicial de Many */
        if (enemy->anim.current_frame == 0 && enemy->anim.anim_timer == 0)
        {
            enemy->direction = (many_x >= enemy->x) ? 1 : 0;
        }

        /* Avanzar frames de emergencia: 0 -> 1 -> 2 -> 3 -> 4 */
        enemy->anim.anim_timer++;
        if (enemy->anim.anim_timer >= enemy->anim.anim_speed)
        {
            enemy->anim.anim_timer = 0;
            enemy->anim.current_frame++;

            if (enemy->anim.current_frame >= enemy->anim.total_frames)
            {
                /* Termina de emerger: pasa a caminar en línea recta hacia la dirección fijada */
                enemy->state = ZOMBIE_STATE_WALKING;
                enemy->velocity_x = enemy->direction ? ZOMBIE_WALK_SPEED : -ZOMBIE_WALK_SPEED;

                /* Animación de caminata: 3 frames (24,48), (48,48), (72,48) */
                enemy->anim.current_frame = 0;
                enemy->anim.total_frames = 3;
                enemy->anim.anim_timer = 0;
                enemy->anim.anim_speed = ZOMBIE_ANIM_SPEED_WALK;
                enemy->anim.u = s_walk_uv[0].u;
                enemy->anim.v = s_walk_uv[0].v;
            }
            else
            {
                enemy->anim.u = s_emerge_uv[enemy->anim.current_frame].u;
                enemy->anim.v = s_emerge_uv[enemy->anim.current_frame].v;
            }
        }
        break;
    }

    case ZOMBIE_STATE_WALKING:
    {
        /*
         * Comportamiento fiel a SotN:
         * Camina en línea recta sin detenerse ni volverse a hundir pacíficamente por distancia.
         * Aplica gravedad si hay desniveles.
         * Si choca lateralmente contra un obstáculo insalvable o cae al vacío del cuarto,
         * entra en ciclo de reaparición dinámica.
         */

        /* Gravedad leve para descender pendientes/escalones */
        if (enemy->velocity_y < 6)
        {
            enemy->velocity_y += 1;
        }

        int32_t x = enemy->x;
        int32_t y = enemy->y;
        int32_t vx = enemy->velocity_x;
        int32_t vy = enemy->velocity_y;
        bool grounded = false;

        if (level)
        {
            level_resolve_entity_collision(level, &x, &y, &vx, &vy,
                                           ZOMBIE_SPRITE_W, ZOMBIE_SPRITE_H,
                                           -1, &grounded);
        }
        else
        {
            x += vx;
            y += vy;
        }

        /* Si colisionó lateralmente con un obstáculo insalvable (vx bloqueado a 0) */
        if (vx == 0 && enemy->velocity_x != 0)
        {
            /* Entra en ciclo de reaparición dinámica */
            enemy->state = ZOMBIE_STATE_RESPAWN_WAIT;
            enemy->respawn_timer = ZOMBIE_RESPAWN_DELAY_FRAMES;
            enemy->active = 0;
            return;
        }

        /* Si cayó al abismo por debajo de los límites del nivel */
        if (y > level_h)
        {
            enemy->state = ZOMBIE_STATE_RESPAWN_WAIT;
            enemy->respawn_timer = ZOMBIE_RESPAWN_DELAY_FRAMES;
            enemy->active = 0;
            return;
        }

        /* Si se sale por los bordes laterales del nivel */
        if (x < -ZOMBIE_SPRITE_W || x > level_w + ZOMBIE_SPRITE_W)
        {
            enemy->state = ZOMBIE_STATE_RESPAWN_WAIT;
            enemy->respawn_timer = ZOMBIE_RESPAWN_DELAY_FRAMES;
            enemy->active = 0;
            return;
        }

        enemy->x = x;
        enemy->y = y;
        enemy->velocity_y = vy;

        /* Ciclo de animación de caminata: 0 -> 1 -> 2 -> 0 */
        enemy->anim.anim_timer++;
        if (enemy->anim.anim_timer >= enemy->anim.anim_speed)
        {
            enemy->anim.anim_timer = 0;
            enemy->anim.current_frame = (enemy->anim.current_frame + 1) % enemy->anim.total_frames;
            enemy->anim.u = s_walk_uv[enemy->anim.current_frame].u;
            enemy->anim.v = s_walk_uv[enemy->anim.current_frame].v;
        }
        break;
    }

    default:
        break;
    }
}

void zombie_render(Enemy *enemy, int32_t cam_x, int32_t cam_y)
{
    if (!enemy || !enemy->active || !s_zombie_tim_loaded)
        return;

    /* Si está esperando para reaparecer, no se dibuja */
    if (enemy->state == ZOMBIE_STATE_RESPAWN_WAIT)
        return;

    int32_t screen_x = enemy->x - cam_x;
    int32_t screen_y = enemy->y - cam_y;

    /* Culling de pantalla: solo renderizar si está dentro del viewport */
    if (screen_x < SCREEN_XRES &&
        screen_x + ZOMBIE_SPRITE_W > 0 &&
        screen_y < SCREEN_YRES &&
        screen_y + ZOMBIE_SPRITE_H > 0)
    {
        /* Efecto de parpadeo visual al recibir daño */
        uint8_t r = 128, g = 128, b = 128;
        if (enemy->invulnerable_timer > 0 && (enemy->invulnerable_timer & 2))
        {
            r = 255;
            g = 64;
            b = 64;
        }

        uint8_t u = (uint8_t)enemy->anim.u;
        uint8_t v = (uint8_t)enemy->anim.v;

        /*
         * Orientación horizontal (rotateu):
         * Los sprites base miran hacia la derecha en la sprite sheet.
         * Si direction == 1 (derecha) -> rotateu = 0 (sin invertir).
         * Si direction == 0 (izquierda) -> rotateu = 1 (invertido horizontalmente).
         */
        render_poly_ft4(
            s_zombie_tim,
            screen_x,
            screen_y,
            ZOMBIE_SPRITE_W,
            ZOMBIE_SPRITE_H,
            u,
            v,
            r, g, b,
            enemy->direction ? 0 : 1, 0);
    }
}

uint8_t zombie_take_damage(Enemy *enemy, uint16_t damage)
{
    if (!enemy || !enemy->active || enemy->invulnerable_timer > 0)
        return 0;

    /*
     * Requisito SotN:
     * Hitbox inactivo o invulnerable mientras está terminando de emerger de la tierra.
     * Solo es vulnerable cuando está caminando (ZOMBIE_STATE_WALKING).
     */
    if (enemy->state != ZOMBIE_STATE_WALKING)
        return 0;

    enemy->hp -= (int16_t)damage;
    enemy->invulnerable_timer = 10; /* Frames de inmunidad/parpadeo */

    if (enemy->hp <= 0)
    {
        enemy->hp = 0;
        /* Al morir, entra en ciclo de reaparición dinámica (SotN) */
        enemy->state = ZOMBIE_STATE_RESPAWN_WAIT;
        enemy->respawn_timer = ZOMBIE_RESPAWN_DELAY_FRAMES;
        enemy->active = 0;
        return 1; /* Notifica derrota para otorgar EXP o efectos */
    }

    return 0;
}
