#include <string.h>
#include "level_generator.h"
#include "../../graphics/graphic.h"
#include "../../graphics/door.h"

/* Helper clamp */
static inline int32_t clamp_i32(int32_t val, int32_t min, int32_t max)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

static LevelTile *level_tile_at(DynamicLevel *level, uint16_t tx, uint16_t ty)
{
    return level->level_orientation == LEVEL_VERTICAL
        ? &level->tiles[tx][ty]
        : &level->tiles[ty][tx];
}

static const LevelTile *level_tile_at_const(
    const DynamicLevel *level, uint16_t tx, uint16_t ty)
{
    return level->level_orientation == LEVEL_VERTICAL
        ? &level->tiles[tx][ty]
        : &level->tiles[ty][tx];
}

void level_init(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles, LEVEL_ORIENTATION level_orientation, CD_TEXTURE *tileset_tim)
{
    if (!level)
        return;

    if (width_tiles == 0)
        width_tiles = 1;
    if (height_tiles == 0)
        height_tiles = 1;

    if (level_orientation == LEVEL_VERTICAL ? width_tiles > LEVEL_MAX_HEIGHT : width_tiles > LEVEL_MAX_WIDTH)
        width_tiles = level_orientation == LEVEL_VERTICAL ? LEVEL_MAX_HEIGHT : LEVEL_MAX_WIDTH;
    if (level_orientation == LEVEL_VERTICAL ? height_tiles > LEVEL_MAX_WIDTH : height_tiles > LEVEL_MAX_HEIGHT)
        height_tiles = level_orientation == LEVEL_VERTICAL ? LEVEL_MAX_WIDTH : LEVEL_MAX_HEIGHT;

    level->width = width_tiles;
    level->height = height_tiles;
    level->pixel_width = (uint32_t)width_tiles * TILE_SIZE;
    level->pixel_height = (uint32_t)height_tiles * TILE_SIZE;
    level->level_orientation = level_orientation;
    level->tileset = tileset_tim;
    level->use_bg_tile = 0;
    memset(&level->bg_tile, 0, sizeof(level->bg_tile));
    memset(level->background, 0, sizeof(level->background));
    level->background_count = 0;

    level->camera.x = 0;
    level->camera.y = 0;
    level->camera.viewport_w = SCREEN_XRES;
    level->camera.viewport_h = SCREEN_YRES;

    memset(level->doors, 0, sizeof(level->doors));
    level->door_count = 0;

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

    LevelTile *tile = level_tile_at(level, tx, ty);
    tile->u = tile_def.u;
    tile->v = tile_def.v;
    tile->collision_type = (uint8_t)tile_def.collision;
    tile->active = 1;
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

void level_clear_doors(DynamicLevel *level)
{
    if (!level)
        return;
    memset(level->doors, 0, sizeof(level->doors));
    level->door_count = 0;
}

int8_t level_add_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open, uint8_t type)
{
    if (!level)
        return -1;

    if (level->door_count >= LEVEL_MAX_DOORS)
        return -1;

    uint8_t idx = level->door_count;
    level->doors[idx].x = x;
    level->doors[idx].y = y;
    level->doors[idx].rotateu = rotateu;
    level->doors[idx].open = open;
    level->doors[idx].active = 1;
    level->doors[idx].current_frame = 0;
    level->doors[idx].timer = 0;
    level->doors[idx].type = type;
    level->doors[idx].target_room = -1;
    level->doors[idx].target_level = -1;

    level->door_count++;
    return (int8_t)idx;
}

LevelDoor *level_get_door(DynamicLevel *level, uint8_t index)
{
    if (!level || index >= level->door_count)
        return NULL;
    return &level->doors[index];
}

LevelDoor *level_get_door_by_type(DynamicLevel *level, uint8_t type)
{
    if (!level)
        return NULL;

    for (uint8_t i = 0; i < level->door_count; i++)
    {
        if (level->doors[i].active && level->doors[i].type == type)
        {
            return &level->doors[i];
        }
    }
    return NULL;
}

LevelDoor *level_get_enter_door(DynamicLevel *level)
{
    return level_get_door_by_type(level, DOOR_TYPE_ENTER);
}

LevelDoor *level_get_exit_door(DynamicLevel *level)
{
    return level_get_door_by_type(level, DOOR_TYPE_EXIT);
}

void level_set_enter_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open)
{
    if (!level)
        return;

    LevelDoor *door = level_get_enter_door(level);
    if (!door)
    {
        level_add_door(level, x, y, rotateu, open, DOOR_TYPE_ENTER);
    }
    else
    {
        door->x = x;
        door->y = y;
        door->rotateu = rotateu;
        door->open = open;
        door->active = 1;
        door->type = DOOR_TYPE_ENTER;
    }
}

void level_set_exit_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open)
{
    if (!level)
        return;

    LevelDoor *door = level_get_exit_door(level);
    if (!door)
    {
        level_add_door(level, x, y, rotateu, open, DOOR_TYPE_EXIT);
    }
    else
    {
        door->x = x;
        door->y = y;
        door->rotateu = rotateu;
        door->open = open;
        door->active = 1;
        door->type = DOOR_TYPE_EXIT;
    }
}

void level_border_add_floor_opening(LevelSkipBorder *border, uint16_t start_tx, uint16_t size_tx)
{
    if (!border || border->floor_openings_count >= LEVEL_MAX_OPENINGS)
        return;

    uint8_t idx = border->floor_openings_count;
    border->floor_openings[idx].start = start_tx;
    border->floor_openings[idx].size = size_tx;
    border->floor_openings[idx].active = 1;
    border->floor_openings_count++;
}

void level_border_add_ceiling_opening(LevelSkipBorder *border, uint16_t start_tx, uint16_t size_tx)
{
    if (!border || border->ceiling_openings_count >= LEVEL_MAX_OPENINGS)
        return;

    uint8_t idx = border->ceiling_openings_count;
    border->ceiling_openings[idx].start = start_tx;
    border->ceiling_openings[idx].size = size_tx;
    border->ceiling_openings[idx].active = 1;
    border->ceiling_openings_count++;
}

void level_border_add_wall_left_opening(LevelSkipBorder *border, uint16_t start_ty, uint16_t size_ty)
{
    if (!border || border->wall_left_openings_count >= LEVEL_MAX_OPENINGS)
        return;

    uint8_t idx = border->wall_left_openings_count;
    border->wall_left_openings[idx].start = start_ty;
    border->wall_left_openings[idx].size = size_ty;
    border->wall_left_openings[idx].active = 1;
    border->wall_left_openings_count++;
}

void level_border_add_wall_right_opening(LevelSkipBorder *border, uint16_t start_ty, uint16_t size_ty)
{
    if (!border || border->wall_right_openings_count >= LEVEL_MAX_OPENINGS)
        return;

    uint8_t idx = border->wall_right_openings_count;
    border->wall_right_openings[idx].start = start_ty;
    border->wall_right_openings[idx].size = size_ty;
    border->wall_right_openings[idx].active = 1;
    border->wall_right_openings_count++;
}

const BorderOpening *level_border_find_opening_at(const BorderOpening *openings, uint8_t count, uint16_t pos)
{
    if (!openings || count == 0)
        return NULL;

    for (uint8_t i = 0; i < count; i++)
    {
        if (openings[i].active && pos >= openings[i].start && pos < (openings[i].start + openings[i].size))
        {
            return &openings[i];
        }
    }
    return NULL;
}

static inline bool is_in_opening(uint16_t pos, const BorderOpening *openings, uint8_t count)
{
    return level_border_find_opening_at(openings, count, pos) != NULL;
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

    /* Paredes laterales (izquierda y derecha) con soporte para huecos/aberturas */
    if (!skip_border.skip_wall_left || !skip_border.skip_wall_rigth) {
        for (uint8_t t = 0; t < wall_thickness && t < w; t++)
        {
            for (uint16_t y = 0; y < h; y++)
            {
                if (!skip_border.skip_wall_left)
                {
                    if (!is_in_opening(y, skip_border.wall_left_openings, skip_border.wall_left_openings_count))
                    {
                        level_set_tile(level, t, y, wall_tile);
                    }
                }
                if (!skip_border.skip_wall_rigth)
                {
                    if (!is_in_opening(y, skip_border.wall_right_openings, skip_border.wall_right_openings_count))
                    {
                        level_set_tile(level, w - 1 - t, y, wall_tile);
                    }
                }
            }
        }
    }

    /* Techo con soporte para huecos/aberturas */
    if (!skip_border.skip_ceiling) {
        for (uint8_t t = 0; t < ceiling_thickness && t < h; t++)
        {
            for (uint16_t x = 0; x < w; x++)
            {
                if (!is_in_opening(x, skip_border.ceiling_openings, skip_border.ceiling_openings_count))
                {
                    level_set_tile(level, x, t, ceiling_tile);
                }
            }
        }
    }

    /* Piso con soporte para huecos/aberturas */
    if (!skip_border.skip_floor) {
        for (uint8_t t = 0; t < floor_thickness && t < h; t++)
        {
            uint16_t y = h - 1 - t;
            for (uint16_t x = 0; x < w; x++)
            {
                if (!is_in_opening(x, skip_border.floor_openings, skip_border.floor_openings_count))
                {
                    level_set_tile(level, x, y, t == floor_thickness - 1 ? floor_tile : filled_tile);
                }
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

void level_add_wall(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t width_tiles, uint16_t height_tiles,
                    TileTextureDef hill_tile,
                    uint8_t use_top_tile, TileTextureDef hill_top_tile,
                    uint8_t use_down_tile, TileTextureDef hill_down_tile)
{
    if (!level)
        return;
    if (tx >= level->width)
        return;

    // Estas propiedades ya se las asigno al iniciarlizar la textura
    // hill_tile.collision = TILE_COLL_SOLID;
    // hill_top_tile.collision = TILE_COLL_SOLID;

    for (uint16_t x=tx; x<tx+width_tiles; x++) {
        for (uint16_t i = 0; i < height_tiles; i++)
        {
            uint16_t y = ty + i;
            if (y >= level->height)
                break;
            if (use_top_tile && i == 0) level_set_tile(level, x, y, hill_top_tile);
            else if (use_down_tile && i == height_tiles - 1) level_set_tile(level, x, y, hill_down_tile);
            else level_set_tile(level, x, y, hill_tile);
        }
    }
}

void level_add_stairway(DynamicLevel *level, const LevelThemeTextures *theme,
                        uint16_t tx, uint16_t ty,
                        uint16_t lower_w, uint16_t lower_h,
                        uint8_t steps, uint16_t base_w,
                        uint8_t with_base, uint8_t with_border,
                        STAIRWAY_DIRECTION direction)
{
    if (!level || !theme)
        return;

    uint16_t base_width = lower_w + steps + base_w;
    uint16_t real_ty = with_base ? (ty - 2) : (ty - 1);

    if (direction == STAIRWAY_LEFT)
    {
        TileTextureDef stair_def = theme->stairwayl_tile;
        stair_def.collision = TILE_COLL_STAIR_LEFT;

        // Plataforma base inferior
        level_add_wall(level, tx, ty - 1, base_width, lower_h,
                       theme->filled_tile, 0, theme->filled_tile, with_base, theme->hillb_tile);

        // Inicio de plataforma (parte baja)
        uint16_t lower_tx = tx + base_w + steps;
        if (lower_w > with_border)
        {
            level_add_wall(level, lower_tx, real_ty, lower_w - with_border, 1,
                           theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile);
        }
        if (with_border && lower_w > 0)
        {
            level_add_wall(level, lower_tx + lower_w - 1, real_ty, 1, 1,
                           theme->filled_tile, 1, theme->hillr_tile, 0, theme->filled_tile);
        }

        // Escalones
        uint16_t step_y = real_ty;
        for (int16_t i = steps - 1, j = 0; i >= 0; i--, j++)
        {
            level_add_wall(level, tx + base_w + i, step_y--, 1, 1 + j,
                           theme->filled_tile, 1, theme->stairwaybl_tile, 0, theme->filled_tile);
            level_add_wall(level, tx + base_w + i, step_y, 1, 1,
                           stair_def, 1, stair_def, 1, theme->filled_tile);
        }

        // Final de plataforma (parte alta)
        level_add_wall(level, tx, step_y, base_w, steps + 1,
                       theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile);
    }
    else if (direction == STAIRWAY_RIGHT)
    {
        TileTextureDef stair_def = theme->stairwayr_tile;
        stair_def.collision = TILE_COLL_STAIR_RIGHT;

        // Plataforma base inferior
        uint16_t base_tx = tx - base_width;
        level_add_wall(level, base_tx, ty - 1, base_width, lower_h,
                       theme->filled_tile, 0, theme->filled_tile, with_base, theme->hillb_tile);

        // Inicio de plataforma (parte baja)
        if (with_border && lower_w > 0)
        {
            level_add_wall(level, base_tx, real_ty, 1, 1,
                           theme->filled_tile, 1, theme->hilll_tile, 0, theme->filled_tile);
        }
        if (lower_w > with_border)
        {
            level_add_wall(level, base_tx + with_border, real_ty, lower_w - with_border, 1,
                           theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile);
        }

        // Escalones
        uint16_t aux_x = base_tx + lower_w;
        uint16_t step_y = real_ty;
        for (int16_t i = 0; i < steps; i++)
        {
            level_add_wall(level, aux_x + i, step_y--, 1, 1 + i,
                           theme->filled_tile, 1, theme->stairwaybr_tile, 0, theme->filled_tile);
            level_add_wall(level, aux_x + i, step_y, 1, 1,
                           stair_def, 1, stair_def, 1, theme->filled_tile);
        }

        // Final de plataforma (parte alta)
        level_add_wall(level, aux_x + steps, step_y, base_w, steps + 1,
                       theme->filled_tile, 1, theme->hillf_tile, 0, theme->filled_tile);
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

void level_generate_base(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles, LEVEL_ORIENTATION level_orientation,
                         CD_TEXTURE *tileset_tim, const LevelThemeTextures *theme,
                         LevelSkipBorder skip_border)
{
    if (!level || !theme)
        return;

    level_init(level, width_tiles, height_tiles, level_orientation, tileset_tim);

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
            const LevelTile *tile = level_tile_at_const(level, (uint16_t)tx, (uint16_t)ty);

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
    for (uint8_t i = 0; i < level->door_count; i++)
    {
        if (level->doors[i].active)
        {
            door_render_instance(level->doors[i].x, level->doors[i].y,
                                 cam_x, cam_y,
                                 level->doors[i].rotateu, level->doors[i].open,
                                 (uint8_t *)&level->doors[i].current_frame,
                                 (uint8_t *)&level->doors[i].timer);
        }
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
    const LevelTile *tile =
    level_tile_at_const(level, (uint16_t)tx, (uint16_t)ty);
    return (TileCollisionType)tile->collision_type;
}

bool level_get_stair_surface_y(const DynamicLevel *level, int32_t x, int32_t feet_y, int32_t *out_floor_y)
{
    if (!level || !out_floor_y)
        return false;

    if (x < 0 || x >= (int32_t)level->pixel_width)
        return false;

    int32_t tx = x / TILE_SIZE;
    int32_t offset_x = x % TILE_SIZE;
    if (offset_x < 0)
        offset_x += TILE_SIZE;

    int32_t center_ty = feet_y / TILE_SIZE;

    /* Comprobar exclusivamente si el tile en tx es escalera */
    for (int32_t dty = -2; dty <= 2; dty++)
    {
        int32_t ty = center_ty + dty;
        if (ty < 0 || ty >= level->height)
            continue;

        TileCollisionType coll = level_get_tile_collision(level, tx, ty);
        if (coll == TILE_COLL_STAIR_LEFT)
        {
            /* Sube hacia la izquierda: base baja a la derecha (offset_x=15 -> y=ty*16+15), alta a la izquierda (offset_x=0 -> y=ty*16) */
            *out_floor_y = (ty * TILE_SIZE) + offset_x;
            return true;
        }
        else if (coll == TILE_COLL_STAIR_RIGHT)
        {
            /* Sube hacia la derecha: base baja a la izquierda (offset_x=0 -> y=ty*16+15), alta a la derecha (offset_x=15 -> y=ty*16) */
            *out_floor_y = (ty * TILE_SIZE) + (TILE_SIZE - 1 - offset_x);
            return true;
        }
    }

    return false;
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
     * Dejamos margen en Y para evitar falsos positivos con piso/techo y escalones */
    int32_t foot_center_x = x + (w / 2);
    int32_t dummy_stair_y = 0;
    bool on_stair = level_get_stair_surface_y(level, foot_center_x, y + h, &dummy_stair_y);

    if (vx > 0)
    {
        /* Movimiento hacia la derecha */
        int32_t check_tx = (new_x + w - 1) / TILE_SIZE;
        int32_t start_ty = (y + 2) / TILE_SIZE;
        int32_t end_ty = (on_stair ? (y + h - 18) : (y + h - 6)) / TILE_SIZE;
        if (end_ty < start_ty)
            end_ty = start_ty;
        bool hit_wall = false;

        for (int32_t ty = start_ty; ty <= end_ty; ty++)
        {
            TileCollisionType coll = level_get_tile_collision(level, check_tx, ty);
            if (coll == TILE_COLL_SOLID)
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
        int32_t end_ty = (on_stair ? (y + h - 18) : (y + h - 6)) / TILE_SIZE;
        if (end_ty < start_ty)
            end_ty = start_ty;
        bool hit_wall = false;

        for (int32_t ty = start_ty; ty <= end_ty; ty++)
        {
            TileCollisionType coll = level_get_tile_collision(level, check_tx, ty);
            if (coll == TILE_COLL_SOLID)
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

        /* Deteccion y aterrizaje en escaleras en 45 grados */
        int32_t foot_center_x = x + (w / 2);
        int32_t stair_y = 0;
        if (level_get_stair_surface_y(level, foot_center_x, new_feet_y, &stair_y))
        {
            /* Si los pies cruzaron o estan sobre la superficie de la rampa */
            if (new_feet_y >= stair_y - 4 && old_feet_y <= stair_y + 12)
            {
                new_y = stair_y - h;
                vy = 0;
                grounded = true;
                hit_floor = true;
            }
        }

        /* Si aterrizó en una escalera o venía de una rampa, NO colisionar con piso plano debajo */
        if (!grounded && hit_floor)
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
        else
        {
            /* Si los pies están dentro de 1 o 2 px por encima del piso plano (por ejemplo al salir
             * de una rampa), ajustar suavemente al piso plano para no perder el suelo */
            int32_t next_support_ty = (support_y + 2) / TILE_SIZE;
            int32_t floor_top = next_support_ty * TILE_SIZE;
            if (support_y >= floor_top - 2 && support_y < floor_top)
            {
                for (int32_t tx = start_tx; tx <= end_tx; tx++)
                {
                    TileCollisionType coll =
                        level_get_tile_collision(level, tx, next_support_ty);

                    if (coll == TILE_COLL_SOLID || (coll == TILE_COLL_ONEWAY && next_support_ty != ignored_oneway_ty))
                    {
                        grounded = true;
                        new_y = floor_top - h;
                        break;
                    }
                }
            }
        }

        /* Soporte si los pies estan directamente sobre una superficie de escalera */
        if (!grounded)
        {
            int32_t foot_center_x = x + (w / 2);
            int32_t stair_y = 0;
            if (level_get_stair_surface_y(level, foot_center_x, support_y, &stair_y))
            {
                if (support_y >= stair_y - 6 && support_y <= stair_y + 6)
                {
                    grounded = true;
                    new_y = stair_y - h;
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
