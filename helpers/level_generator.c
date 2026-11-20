#include <stdio.h>
#include "level_generator.h"
#include "../graphics/graphic.h"
#include "../graphics/door.h"
#include <string.h>

/* Helper clamp */
static inline int32_t clamp_i32(int32_t val, int32_t min, int32_t max)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

void level_init(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles, CD_TEXTURE *tileset_tim)
{
    if (!level)
        return;

    if (width_tiles == 0)
        width_tiles = 1;
    if (height_tiles == 0)
        height_tiles = 1;

    if (width_tiles > LEVEL_MAX_WIDTH)
        width_tiles = LEVEL_MAX_WIDTH;
    if (height_tiles > LEVEL_MAX_HEIGHT)
        height_tiles = LEVEL_MAX_HEIGHT;

    level->width = width_tiles;
    level->height = height_tiles;
    level->pixel_width = (uint32_t)width_tiles * TILE_SIZE;
    level->pixel_height = (uint32_t)height_tiles * TILE_SIZE;
    level->tileset = tileset_tim;
    level->use_bg_tile = 0;
    memset(&level->bg_tile, 0, sizeof(level->bg_tile));
    memset(level->background, 0, sizeof(level->background));
    level->background_count = 0;

    level->camera.x = 0;
    level->camera.y = 0;
    level->camera.viewport_w = SCREEN_XRES;
    level->camera.viewport_h = SCREEN_YRES;

    memset(&level->enter_door, 0, sizeof(LevelDoor));
    memset(&level->exit_door, 0, sizeof(LevelDoor));

    level_clear(level);
}

void level_clear(DynamicLevel *level)
{
    if (!level)
        return;
    memset(level->tiles, 0, sizeof(level->tiles));
}

void level_set_tile(DynamicLevel *level, uint16_t tx, uint16_t ty, TileTextureDef tile_def)
{
    if (!level)
        return;
    if (tx >= level->width || ty >= level->height)
        return;

    level->tiles[ty][tx].u = tile_def.u;
    level->tiles[ty][tx].v = tile_def.v;
    level->tiles[ty][tx].collision_type = (uint8_t)tile_def.collision;
    level->tiles[ty][tx].active = 1;
}

void level_set_background(DynamicLevel *level,
                          CD_TEXTURE background_tim[BACKGROUND_MAX],
                          uint8_t count)
{
    if (!level || !background_tim)
        return;

    if (count > BACKGROUND_MAX)
        count = BACKGROUND_MAX;

    memcpy(level->background, background_tim, sizeof(level->background));
    level->background_count = count;
}

void level_set_enter_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open)
{
    if (!level)
        return;

    level->enter_door.x = x;
    level->enter_door.y = y;
    level->enter_door.rotateu = rotateu;
    level->enter_door.open = open;
    level->enter_door.active = 1;
}

void level_set_exit_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open)
{
    if (!level)
        return;

    level->exit_door.x = x;
    level->exit_door.y = y;
    level->exit_door.rotateu = rotateu;
    level->exit_door.open = open;
    level->exit_door.active = 1;
}

void level_build_border(DynamicLevel *level,
                        TileTextureDef floor_tile, uint8_t floor_thickness,
                        TileTextureDef ceiling_tile, uint8_t ceiling_thickness,
                        TileTextureDef wall_tile, uint8_t wall_thickness,
                        TileTextureDef filled_tile,
                        LevelSkipBorder skip_border)
{
    if (!level)
        return;

    uint16_t w = level->width;
    uint16_t h = level->height;

    if (!skip_border.skip_wall_left || !skip_border.skip_wall_rigth) {
        /* Paredes laterales (izquierda y derecha) */
        for (uint8_t t = 0; t < wall_thickness && t < w; t++)
        {
            for (uint16_t y = 0; y < h; y++)
            {
                if (!skip_border.skip_wall_left) level_set_tile(level, t, y, wall_tile);         /* Izquierda */
                if (!skip_border.skip_wall_rigth) level_set_tile(level, w - 1 - t, y, wall_tile); /* Derecha */
            }
        }
    }

    if (!skip_border.skip_ceiling) {
        /* Techo */
        for (uint8_t t = 0; t < ceiling_thickness && t < h; t++)
        {
            for (uint16_t x = 0; x < w; x++)
            {
                level_set_tile(level, x, t, ceiling_tile);
            }
        }
    }

    if (!skip_border.skip_floor) {
        /* Piso */
        for (uint8_t t = 0; t < floor_thickness && t < h; t++)
        {
            uint16_t y = h - 1 - t;
            for (uint16_t x = 0; x < w; x++)
            {
                level_set_tile(level, x, y, t == floor_thickness - 1 ? floor_tile : filled_tile);
            }
        }
    }
}

void level_add_platform(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t length_tiles,
                        TileTextureDef pltfrml_tile,
                        TileTextureDef pltfrmf_tile,
                        TileTextureDef pltfrmr_tile,
                        bool is_oneway)
{
    if (!level)
        return;
    if (ty >= level->height)
        return;

    pltfrml_tile.collision = is_oneway ? TILE_COLL_ONEWAY : TILE_COLL_SOLID;
    pltfrmf_tile.collision = is_oneway ? TILE_COLL_ONEWAY : TILE_COLL_SOLID;
    pltfrmr_tile.collision = is_oneway ? TILE_COLL_ONEWAY : TILE_COLL_SOLID;

    for (uint16_t i = 0; i < length_tiles; i++)
    {
        uint16_t x = tx + i;
        if (x >= level->width)
            break;
        level_set_tile(level, x, ty, i == 0 ? pltfrml_tile : i == length_tiles - 1 ? pltfrmr_tile
                                                                                   : pltfrmf_tile);
    }
}

void level_add_wall(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t height_tiles,
                    TileTextureDef wall_tile)
{
    printf("Dibujando pared de tx: %d, ty: %d por tantos tiles de alto: %d\n", tx, ty, height_tiles);
    if (!level)
        return;
    if (tx >= level->width)
        return;

    wall_tile.collision = TILE_COLL_SOLID;

    for (uint16_t i = 0; i < height_tiles; i++)
    {
        uint16_t y = ty + i;
        if (y >= level->height)
            break;
        level_set_tile(level, tx, y, wall_tile);
    }
}

void level_fill_rect(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t w_tiles, uint16_t h_tiles,
                     TileTextureDef tile_def)
{
    if (!level)
        return;

    for (uint16_t y = 0; y < h_tiles; y++)
    {
        uint16_t curr_y = ty + y;
        if (curr_y >= level->height)
            break;

        for (uint16_t x = 0; x < w_tiles; x++)
        {
            uint16_t curr_x = tx + x;
            if (curr_x >= level->width)
                break;

            level_set_tile(level, curr_x, curr_y, tile_def);
        }
    }
}

void level_generate_base(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles,
                         CD_TEXTURE *tileset_tim, const LevelThemeTextures *theme,
                         LevelSkipBorder skip_border)
{
    if (!level || !theme)
        return;

    level_init(level, width_tiles, height_tiles, tileset_tim);

    level->bg_tile = theme->bg_tile;
    level->use_bg_tile = theme->use_bg_tile;

    /* 1. Construir perimetro seguro: piso de 3 tiles, techo de 2 tiles, paredes de 2 tiles */
    level_build_border(level,
                       theme->floor_tile, 3,
                       theme->ceiling_tile, 2,
                       theme->wall_tile, 2,
                       theme->filled_tile,
                       skip_border);
}

/* ========================================================================= */
/* CAMARA Y SCROLL                                                           */
/* ========================================================================= */

void level_update_camera(DynamicLevel *level, int32_t target_x, int32_t target_y, int32_t target_w, int32_t target_h)
{
    if (!level)
        return;

    /* Centrar la camara en el objetivo */
    int32_t center_x = target_x + (target_w / 2);
    int32_t center_y = target_y + (target_h / 2);

    int32_t cam_x = center_x - (level->camera.viewport_w / 2);
    int32_t cam_y = center_y - (level->camera.viewport_h / 2);

    level_set_camera_pos(level, cam_x, cam_y);
}

void level_get_camera_pos(const DynamicLevel *level, int32_t *out_cam_x, int32_t *out_cam_y)
{
    if (!level)
        return;
    if (out_cam_x)
        *out_cam_x = level->camera.x;
    if (out_cam_y)
        *out_cam_y = level->camera.y;
}

void level_set_camera_pos(DynamicLevel *level, int32_t cam_x, int32_t cam_y)
{
    if (!level)
        return;

    int32_t max_cam_x = (int32_t)level->pixel_width - level->camera.viewport_w;
    int32_t max_cam_y = (int32_t)level->pixel_height - level->camera.viewport_h;

    if (max_cam_x < 0)
        max_cam_x = 0;
    if (max_cam_y < 0)
        max_cam_y = 0;

    level->camera.x = clamp_i32(cam_x, 0, max_cam_x);
    level->camera.y = clamp_i32(cam_y, 0, max_cam_y);
}

/* ========================================================================= */
/* RENDERIZADO CON FRUSTUM CULLING                                           */
/* ========================================================================= */

static int background_uv_origin(
    const CD_TEXTURE *texture,
    uint8_t *u,
    uint8_t *v)
{
    uint32_t pixels_per_word;

    switch (texture->mode & 3)
    {
    case 0:
        pixels_per_word = 4;
        break;
    case 1:
        pixels_per_word = 2;
        break;
    case 2:
        pixels_per_word = 1;
        break;
    default:
        return 0;
    }

    *u = (uint8_t)((texture->image_x & 63u) * pixels_per_word);
    *v = (uint8_t)(texture->image_y & 255u);
    return 1;
}

static void render_background_row(
    const DynamicLevel *level,
    uint8_t first_index,
    int8_t alternate_index,
    int32_t y,
    uint16_t parallax_x_q8)
{
    const int32_t image_width = 128;
    const int32_t image_height = 128;

    int32_t scroll_x =
        (int32_t)(((int64_t)level->camera.x * parallax_x_q8) >> 8);
    int32_t first_tile = scroll_x / image_width;
    int32_t offset_x = scroll_x % image_width;

    for (int32_t x = -offset_x, tile = 0;
         x < level->camera.viewport_w;
         x += image_width, tile++)
    {
        uint8_t texture_index = first_index;

        if (alternate_index >= 0 && ((first_tile + tile) & 1))
            texture_index = (uint8_t)alternate_index;

        int32_t draw_width = image_width;
        if (x + draw_width > level->camera.viewport_w)
            draw_width = level->camera.viewport_w - x;

        CD_TEXTURE texture = level->background[texture_index];
        uint8_t u, v;

        if (!background_uv_origin(&texture, &u, &v))
            return;

        render_poly_ft4_z(
            texture,
            x, y, (uint32_t)draw_width, (uint32_t)image_height,
            u, v, 128, 128, 128,
            0, 0, 2);
    }
}

static void render_background_tile_fill(const DynamicLevel *level, int32_t start_y)
{
    CD_TEXTURE tileset = *level->tileset;

    for (int32_t y = start_y; y < level->camera.viewport_h; y += TILE_SIZE)
    {
        int32_t draw_h = level->camera.viewport_h - y;
        if (draw_h > TILE_SIZE)
            draw_h = TILE_SIZE;

        for (int32_t x = 0; x < level->camera.viewport_w; x += TILE_SIZE)
        {
            int32_t draw_w = level->camera.viewport_w - x;
            if (draw_w > TILE_SIZE)
                draw_w = TILE_SIZE;

            render_poly_ft4_z(
                tileset,
                x, y, (uint32_t)draw_w, (uint32_t)draw_h,
                level->bg_tile.u, level->bg_tile.v,
                128, 128, 128,
                0, 0, 3);
        }
    }
}

void level_render(const DynamicLevel *level)
{
    if (!level || !level->tileset)
        return;

    if (level->use_bg_tile)
    {
        if (level->background_count == 0)
        {
            render_background_tile_fill(level, 0);
        }
        else if (level->background_count == 1 ||
                level->background_count == 2)
        {
            render_background_tile_fill(level, 128);
        }
    }

    /* 1. Imagen de fondo */
    if (level->background_count >= 1)
    {
        switch (level->background_count)
        {
        case 1:
            render_background_row(level, 0, -1, 0, 64);
            break;

        case 2:
            render_background_row(level, 0, -1, 0, 64);
            render_background_row(level, 1, -1, 128, 128);
            break;

        case 3:
            render_background_row(level, 0, 1, 0, 64);
            render_background_row(level, 2, -1, 128, 128);
            break;

        case 4:
            render_background_row(level, 0, 2, 0, 128);
            render_background_row(level, 1, 3, 128, 128);
            break;

        default:
            break;
        }
    }

    int32_t cam_x = level->camera.x;
    int32_t cam_y = level->camera.y;
    CD_TEXTURE tim = *(level->tileset);

    /* 2. Tiles visibles del nivel */
    int32_t start_col = cam_x / TILE_SIZE;
    int32_t end_col =
        (cam_x + level->camera.viewport_w + TILE_SIZE - 1) / TILE_SIZE;
    int32_t start_row = cam_y / TILE_SIZE;
    int32_t end_row =
        (cam_y + level->camera.viewport_h + TILE_SIZE - 1) / TILE_SIZE;

    if (start_col < 0)
        start_col = 0;
    if (end_col >= level->width)
        end_col = level->width - 1;
    if (start_row < 0)
        start_row = 0;
    if (end_row >= level->height)
        end_row = level->height - 1;

    for (int32_t ty = start_row; ty <= end_row; ty++)
    {
        int32_t screen_y = ty * TILE_SIZE - cam_y;

        for (int32_t tx = start_col; tx <= end_col; tx++)
        {
            const LevelTile *tile = &level->tiles[ty][tx];

            if (!tile->active)
                continue;

            int32_t screen_x = tx * TILE_SIZE - cam_x;

            render_poly_ft4_z(tim,
                              screen_x, screen_y,
                              TILE_SIZE, TILE_SIZE,
                              tile->u, tile->v,
                              128, 128, 128,
                              0, 0, !tile->collision_type == TILE_COLL_SOLID);
        }
    }

    /* 3. Renderizar puertas configuradas en el nivel */
    if (level->enter_door.active)
    {
        door_render_instance(level->enter_door.x, level->enter_door.y,
                             cam_x, cam_y,
                             level->enter_door.rotateu, level->enter_door.open,
                             (uint8_t *)&level->enter_door.current_frame,
                             (uint8_t *)&level->enter_door.timer);
    }

    if (level->exit_door.active)
    {
        door_render_instance(level->exit_door.x, level->exit_door.y,
                             cam_x, cam_y,
                             level->exit_door.rotateu, level->exit_door.open,
                             (uint8_t *)&level->exit_door.current_frame,
                             (uint8_t *)&level->exit_door.timer);
    }
}

/* ========================================================================= */
/* COLISIONES                                                                */
/* ========================================================================= */

TileCollisionType level_get_tile_collision(const DynamicLevel *level, int32_t tx, int32_t ty)
{
    if (!level)
        return TILE_COLL_SOLID;
    if (tx < 0 || tx >= level->width || ty < 0 || ty >= level->height)
    {
        return TILE_COLL_SOLID; /* Fuera de limites cuenta como solido */
    }

    return (TileCollisionType)level->tiles[ty][tx].collision_type;
}

bool level_point_is_solid(const DynamicLevel *level, int32_t world_x, int32_t world_y)
{
    if (!level)
        return true;

    if (world_x < 0 || world_x >= (int32_t)level->pixel_width ||
        world_y < 0 || world_y >= (int32_t)level->pixel_height)
    {
        return true;
    }

    int32_t tx = world_x / TILE_SIZE;
    int32_t ty = world_y / TILE_SIZE;

    return level_get_tile_collision(level, tx, ty) == TILE_COLL_SOLID;
}

bool level_check_box_collision(const DynamicLevel *level, int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (!level)
        return true;

    /* Fuera de limites */
    if (x < 0 || (x + w) > (int32_t)level->pixel_width ||
        y < 0 || (y + h) > (int32_t)level->pixel_height)
    {
        return true;
    }

    int32_t start_tx = x / TILE_SIZE;
    int32_t end_tx = (x + w - 1) / TILE_SIZE;
    int32_t start_ty = y / TILE_SIZE;
    int32_t end_ty = (y + h - 1) / TILE_SIZE;

    for (int32_t ty = start_ty; ty <= end_ty; ty++)
    {
        for (int32_t tx = start_tx; tx <= end_tx; tx++)
        {
            if (level_get_tile_collision(level, tx, ty) == TILE_COLL_SOLID)
            {
                return true;
            }
        }
    }

    return false;
}

void level_resolve_entity_collision(const DynamicLevel *level,
                                    int32_t *io_x, int32_t *io_y,
                                    int32_t *io_vx, int32_t *io_vy,
                                    int32_t w, int32_t h,
                                    int32_t ignored_oneway_ty,
                                    bool *is_grounded)
{
    if (!level || !io_x || !io_y || !io_vx || !io_vy)
        return;

    int32_t x = *io_x;
    int32_t y = *io_y;
    int32_t vx = *io_vx;
    int32_t vy = *io_vy;
    bool grounded = false;

    /* 1. Limites estrictos del mapa */
    int32_t max_x = (int32_t)level->pixel_width - w;
    int32_t max_y = (int32_t)level->pixel_height - h;

    /* 2. Movimiento horizontal y resolucion contra paredes */
    int32_t new_x = x + vx;
    if (new_x < 0)
    {
        new_x = 0;
        vx = 0;
    }
    else if (new_x > max_x)
    {
        new_x = max_x;
        vx = 0;
    }

    /* Verificamos si la nueva posicion X colisiona con solidos
     * Dejamos 2 px de holgura en Y (cabeza y pies) para evitar falsos positivos con piso/techo */
    if (vx > 0)
    {
        /* Movimiento hacia la derecha */
        int32_t check_tx = (new_x + w - 1) / TILE_SIZE;
        int32_t start_ty = (y + 2) / TILE_SIZE;
        int32_t end_ty = (y + h - 3) / TILE_SIZE;
        if (end_ty < start_ty)
            end_ty = start_ty;
        bool hit_wall = false;

        for (int32_t ty = start_ty; ty <= end_ty; ty++)
        {
            if (level_get_tile_collision(level, check_tx, ty) == TILE_COLL_SOLID)
            {
                hit_wall = true;
                break;
            }
        }

        if (hit_wall)
        {
            new_x = (check_tx * TILE_SIZE) - w;
            vx = 0;
        }
    }
    else if (vx < 0)
    {
        /* Movimiento hacia la izquierda */
        int32_t check_tx = new_x / TILE_SIZE;
        int32_t start_ty = (y + 2) / TILE_SIZE;
        int32_t end_ty = (y + h - 3) / TILE_SIZE;
        if (end_ty < start_ty)
            end_ty = start_ty;
        bool hit_wall = false;

        for (int32_t ty = start_ty; ty <= end_ty; ty++)
        {
            if (level_get_tile_collision(level, check_tx, ty) == TILE_COLL_SOLID)
            {
                hit_wall = true;
                break;
            }
        }

        if (hit_wall)
        {
            new_x = (check_tx + 1) * TILE_SIZE;
            vx = 0;
        }
    }
    x = new_x;

    /* 3. Movimiento vertical y resolucion contra techo y piso/plataformas */
    int32_t new_y = y + vy;
    if (new_y < 0)
    {
        new_y = 0;
        vy = 0;
    }
    else if (new_y > max_y)
    {
        new_y = max_y;
        vy = 0;
        grounded = true;
    }

    /* Margen en X para chequear colisiones verticales y no atorarse en esquinas de tiles */
    int32_t start_tx = (x + 4) / TILE_SIZE;
    int32_t end_tx = (x + w - 5) / TILE_SIZE;
    if (end_tx < start_tx)
        end_tx = start_tx;

    if (vy < 0)
    {
        /* Moviendose hacia arriba: colision contra techo */
        int32_t check_ty = new_y / TILE_SIZE;
        bool hit_ceiling = false;

        for (int32_t tx = start_tx; tx <= end_tx; tx++)
        {
            if (level_get_tile_collision(level, tx, check_ty) == TILE_COLL_SOLID)
            {
                hit_ceiling = true;
                break;
            }
        }

        if (hit_ceiling)
        {
            new_y = (check_ty + 1) * TILE_SIZE;
            vy = 0;
        }
    }
    else
    {
        /* Cayendo o en el suelo (vy >= 0): colision contra piso o plataforma one-way */
        int32_t old_feet_y = y + h - 1;
        int32_t new_feet_y = new_y + h - 1;
        int32_t check_ty = new_feet_y / TILE_SIZE;
        bool hit_floor = false;

        for (int32_t tx = start_tx; tx <= end_tx; tx++)
        {
            TileCollisionType coll = level_get_tile_collision(level, tx, check_ty);

            if (coll == TILE_COLL_SOLID)
            {
                hit_floor = true;
                break;
            }
            else if (coll == TILE_COLL_ONEWAY && check_ty != ignored_oneway_ty)
            {
                /* Para plataformas semi-solidas (one-way), solo colisionamos si los pies
                 * del personaje estaban en o por encima del borde superior de la plataforma
                 * en el frame previo */
                int32_t plat_top_y = check_ty * TILE_SIZE;
                if (old_feet_y <= plat_top_y + 4 && new_feet_y >= plat_top_y)
                {
                    hit_floor = true;
                    break;
                }
            }
        }

        if (hit_floor)
        {
            new_y = (check_ty * TILE_SIZE) - h;
            vy = 0;
            grounded = true;
        }
    }

    if (!grounded && vy >= 0)
    {
        int32_t support_y = new_y + h;

        /* Los pies deben estar justo sobre el borde superior de un tile. */
        if ((support_y % TILE_SIZE) == 0)
        {
            int32_t support_ty = support_y / TILE_SIZE;

            for (int32_t tx = start_tx; tx <= end_tx; tx++)
            {
                TileCollisionType coll =
                    level_get_tile_collision(level, tx, support_ty);

                if (coll == TILE_COLL_SOLID || (coll == TILE_COLL_ONEWAY && support_ty != ignored_oneway_ty))
                {
                    grounded = true;
                    break;
                }
            }
        }
    }

    y = new_y;

    *io_x = x;
    *io_y = y;
    *io_vx = vx;
    *io_vy = vy;
    if (is_grounded)
        *is_grounded = grounded;
}
