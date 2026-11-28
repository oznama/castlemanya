#include "enemy.h"
#include "bat.h"
#include "zombie.h"

void enemy_system_init(void)
{
    bat_init();
    zombie_init();
}

void enemy_init(Enemy *enemy, EnemyType type, int32_t x, int32_t y)
{
    if (!enemy)
        return;

    enemy->type = type;
    enemy->initial_x = x;
    enemy->initial_y = y;
    enemy->hit_this_attack = 0;

    switch (type)
    {
    case ENEMY_BAT:
        bat_setup(enemy, x, y);
        break;
    case ENEMY_ZOMBIE:
        zombie_setup(enemy, x, y);
        break;
    default:
        enemy->active = 0;
        break;
    }
}

void enemy_reset(Enemy *enemy)
{
    if (!enemy || enemy->type == ENEMY_NONE)
        return;

    enemy_init(enemy, enemy->type, enemy->initial_x, enemy->initial_y);
}

void enemy_update(Enemy *enemy, int32_t many_x, int32_t many_y, int32_t level_w, int32_t level_h)
{
    if (!enemy)
        return;

    /* Para enemigos que admiten ciclo de reaparición aunque estén inactivos (ej. ZOMBIE) */
    if (!enemy->active && enemy->type != ENEMY_ZOMBIE)
        return;

    if (enemy->invulnerable_timer > 0)
    {
        enemy->invulnerable_timer--;
    }

    switch (enemy->type)
    {
    case ENEMY_BAT:
        bat_update(enemy, many_x, many_y, level_w, level_h);
        break;
    case ENEMY_ZOMBIE:
        zombie_update(enemy, many_x, many_y, level_w, level_h);
        break;
    default:
        break;
    }
}

void enemy_render(Enemy *enemy, int32_t cam_x, int32_t cam_y)
{
    if (!enemy || !enemy->active)
        return;

    switch (enemy->type)
    {
    case ENEMY_BAT:
        bat_render(enemy, cam_x, cam_y);
        break;
    case ENEMY_ZOMBIE:
        zombie_render(enemy, cam_x, cam_y);
        break;
    default:
        break;
    }
}

uint8_t enemy_take_damage(Enemy *enemy, uint16_t damage)
{
    if (!enemy || !enemy->active)
        return 0;

    switch (enemy->type)
    {
    case ENEMY_ZOMBIE:
        return zombie_take_damage(enemy, damage);
    default:
        break;
    }

    if (enemy->invulnerable_timer > 0)
        return 0;

    enemy->hp -= (int16_t)damage;
    enemy->invulnerable_timer = 10; /* Frames de parpadeo/inmunidad */

    if (enemy->hp <= 0)
    {
        enemy->hp = 0;
        enemy->active = 0;
        return 1; /* Derrotado */
    }

    return 0;
}
