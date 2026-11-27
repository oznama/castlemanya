#include <stdio.h>
#include <string.h>

#include "level_manager.h"
#include "../collisions.h"
#include "../../gameplay/many.h"
#include "../../graphics/graphic.h"
#include "../../helpers/utils.h"

#include "../../world/level01.h"
#include "../../world/level02.h"
#include "../../world/level06.h"

/* Nivel dinámico alojado estáticamente en memoria BSS (evita ~8.5KB en el stack) */
static DynamicLevel s_current_level;

/* Caché de texturas para evitar lecturas redundantes del CD */
static CD_TEXTURE s_tileset_tim;
static char s_cached_tilename[24] = "";
static CD_TEXTURE s_background_tim[BACKGROUND_MAX];
static uint8_t s_background_count = 0;
static uint8_t s_cached_level = 0;

/* Texturas base de tema por defecto (almacenadas como constante estática) */
static const LevelThemeTextures s_default_theme = {
    .floor_tile = {.u = 0, .v = 0, .collision = TILE_COLL_SOLID},
    .filled_tile = {.u = 0, .v = 16, .collision = TILE_COLL_SOLID},
    .wall_tile = {.u = 32, .v = 0, .collision = TILE_COLL_SOLID},
    .ceiling_tile = {.u = 16, .v = 0, .collision = TILE_COLL_SOLID},
    .pltfrml_tile = {.u = 16, .v = 16, .collision = TILE_COLL_ONEWAY},
    .pltfrmf_tile = {.u = 32, .v = 16, .collision = TILE_COLL_ONEWAY},
    .pltfrmr_tile = {.u = 48, .v = 16, .collision = TILE_COLL_ONEWAY},
    .hilll_tile = {.u = 48, .v = 32, .collision = TILE_COLL_SOLID},
    .hillf_tile = {.u = 32, .v = 32, .collision = TILE_COLL_SOLID},
    .hillb_tile = {.u = 32, .v = 48, .collision = TILE_COLL_SOLID},
    .hillr_tile = {.u = 48, .v = 48, .collision = TILE_COLL_SOLID},
    .stairwayl_tile = {.u = 0, .v = 32, .collision = TILE_COLL_STAIR_LEFT},
    .stairwaybl_tile = {.u = 0, .v = 48, .collision = TILE_COLL_SOLID},
    .stairwayr_tile = {.u = 16, .v = 32, .collision = TILE_COLL_STAIR_RIGHT},
    .stairwaybr_tile = {.u = 16, .v = 48, .collision = TILE_COLL_SOLID},
    .bg_tile = {.u = 48, .v = 0, .collision = TILE_COLL_NONE},
    .use_bg_tile = 1
};

LEVEL_WORLD LEVELS_INFO[LEVELS_COUNT] = {
    [0] = {
        .tilename = "TLSTEXT",.bgname = {"BGEXT", "BGCVRNG"},
		.level_data = {
			[0] = {
				.width_tiles = 40, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
                .background_count = 1, .background_indices = {0},
                .miscObjects = {
                    [0] = {.type = MISC_CANDLE, .active = 1, .x = 160, .y = 80, .r = 128, .g = 128, .b = 128},
                    [1] = {.type = MISC_CANDLE, .active = 1, .x = 320, .y = 80, .r = 128, .g = 128, .b = 128},
                    [2] = {.type = MISC_CANDLE, .active = 1, .x = 480, .y = 80, .r = 128, .g = 128, .b = 128}
                },
				.skip_border = {
					.skip_wall_left = 1, .skip_ceiling = 1,
					.wall_right_openings = {
						{ .start = 0, .size = 15, .active = 1, .target_room = 2, .target_x = 34, .target_y = 32 }
					},
					.wall_right_openings_count = 1,
				},
			},
			[1] = {
				.width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL,
                .miscObjects = {
                    [0] = {.type = MISC_CANDLE, .active = 1, .x = 300, .y = 230, .r = 128, .g = 128, .b = 128},
                },
				.skip_border = {
					.skip_ceiling = 1,
					.skip_floor = 1,
					.wall_left_openings = {
						{ .start = 0, .size = 5, .active = 1, .target_room = 1, .target_x = 600, .target_y = 144 }
					},
					.wall_left_openings_count = 1,
					.wall_right_openings = {
						{ .start = 0, .size = 5, .active = 1, .target_room = 10, .target_x = 34, .target_y = 144},
						{ .start = 19, .size = 5, .active = 1, .target_room = 3, .target_x = 34, .target_y = 144 }
					},
					.wall_right_openings_count = 2,
                    .floor_openings = {
                        { .start = 2, .size = 5, .active = 1, .target_room = 4, .target_x = 480, .target_y = 16 }
                    },
                    .floor_openings_count = 1
				}
			},
			[2] = {
				.width_tiles = 20, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
                .background_count = 1, .background_indices = {1},
				.skip_border = {
					.wall_left_openings = {
						{ .start = 7, .size = 5, .active = 1, .target_room = 2, .target_x = 600, .target_y = 672 }
					},
					.wall_left_openings_count = 1,
				},
			},
            [3] = {
                .width_tiles = 60, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL,
                .background_count = 1, .background_indices = {1},
                .enemies = {
                    [0] = {.type = ENEMY_BAT, .active = 1, .x = 180, .y = 16},
                    [1] = {.type = ENEMY_BAT, .active = 1, .x = 320, .y = 16},
                    [2] = {.type = ENEMY_BAT, .active = 1, .x = 500, .y = 208},
                    [3] = {.type = ENEMY_BAT, .active = 1, .x = 600, .y = 208},
                    [4] = {.type = ENEMY_BAT, .active = 1, .x = 750, .y = 208}
                },
                .skip_border = {
                    .ceiling_openings = {
                        { .start = 30, .size = 5, .active = 1, .target_room = 2, .target_x = 120, .target_y = 300}
                    },
                    .ceiling_openings_count = 1,
                    .wall_left_openings = {
                        { .start = 2, .size = 5, .active = 1, .target_room = 5, .target_x = 370, .target_y = 48 }
                    },
                    .wall_left_openings_count = 1
                }
            },
            [4] = {
                .width_tiles = 25, .height_tiles = 59, .level_orientation = LEVEL_VERTICAL,
                .background_count = 1, .background_indices = {1},
                .skip_border = {
                    .wall_right_openings = {
                        { .start = 1, .size = 5, .active = 1, .target_room = 4, .target_x = 12, .target_y = 64 },
                        { .start = 51, .size = 5, .active = 1, .target_room = 6, .target_x = 10, .target_y = 48 }
                    },
                    .wall_right_openings_count = 2
                }
            },
            [5] = {
                .width_tiles = 20, .height_tiles = 89, .level_orientation = LEVEL_VERTICAL,
                .background_count = 1, .background_indices = {1},
                .skip_border = {
                    .wall_left_openings = {
                        { .start = 1, .size = 5, .active = 1, .target_room = 5, .target_x = 368, .target_y = 848 },
                        { .start = 81, .size = 5, .active = 1, .target_room = 7, .target_x = 612, .target_y = 544 }
                    },
                    .wall_left_openings_count = 2
                }
            },
            [6] = {
                .width_tiles = 40, .height_tiles = 74, .level_orientation = LEVEL_VERTICAL,
                .background_count = 1, .background_indices = {1},
                .skip_border = {
                    .wall_right_openings = {
                        { .start = 32, .size = 5, .active = 1, .target_room = 6, .target_x = 2, .target_y = 1328 },
                        { .start = 43, .size = 5, .active = 1, .target_room = 8, .target_x = 32, .target_y = 128 }
                    },
                    .wall_right_openings_count = 2
                }
            },
            [7] = {
                .width_tiles = 128, .height_tiles = 64, .level_orientation = LEVEL_HORIZONTAL,
                .background_count = 1, .background_indices = {1},
                .skip_border = {
                    .wall_left_openings = {
						{ .start = 1, .size = 5, .active = 1, .target_room = 7, .target_x = 610, .target_y = 720 },
                        { .start = 56, .size = 5, .active = 1, .target_room = 9, .target_x = 448, .target_y = 144 }
					},
					.wall_left_openings_count = 2
                }
            },
            [8] = {
                .width_tiles = 30, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
                .background_count = 1, .background_indices = {1},
                .miscObjects = {
                    [0] = {.type = RELIC_VAMPIRE_KILLER, .x = 48, .y = 140, .vertical_direction = 1, .active = 1}
                },
                .skip_border = {
                    .wall_right_openings = {
                        { .start = 7, .size = 5, .active = 1, .target_room = 8, .target_x = 16, .target_y = 928 }
                    },
                    .wall_right_openings_count = 1
                }
            },
			[9] = {
				.width_tiles = 40, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
                .background_count = 1, .background_indices = {0},
				.skip_border = {
					.skip_wall_left = 1,
					.skip_ceiling = 1,
					.wall_left_openings = {
						{ .start = 0, .size = 15, .active = 1, .target_room = 2, .target_x = 564, .target_y = 32 }
					},
					.wall_left_openings_count = 1,
                    // TODO Agregar esta config cuando many obtenga el Vampire killer ó pague 5,000
					// .wall_right_openings = {
					// 	{ .start = 7, .size = 5, .active = 1 }
					// },
					// .wall_right_openings_count = 1,
				},
			}
		},
		.rooms = LEVEL_01_ROOMS
	},
    [1] = {
        .tilename = "TLSTENT", .bgname = {"BGENT1", "BGENT2", "BGENT3", "BGENT4"},
        .enter_door = {.x = 28, .y = 111, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 111, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			[0] = {
				.width_tiles = 128, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
                .miscObjects = {
                    [0] = {.type = MISC_CANDLE, .active = 1, .x = 320, .y = 80, .r = 128, .g = 128, .b = 128},
                    [1] = {.type = MISC_CANDLE, .active = 1, .x = 960, .y = 80, .r = 128, .g = 128, .b = 128},
                    [2] = {.type = MISC_CANDLE, .active = 1, .x = 1600, .y = 80, .r = 128, .g = 128, .b = 128},
                },
				.skip_border = {
					.wall_right_openings = {
						{ .start = 7, .size = 5, .active = 1 }
					},
					.wall_right_openings_count = 1
				}
			},
			[1] = {
				.width_tiles = 30, .height_tiles = 40, .level_orientation = LEVEL_HORIZONTAL,
				.skip_border = {
					.wall_left_openings = {
						{ .start = 32, .size = 5, .active = 1}
					},
					.wall_left_openings_count = 1,
					.wall_right_openings = {
						{ .start = 1, .size = 5, .active = 1},
					},
					.wall_right_openings_count = 1
				}
			},
			[2] = {
				.width_tiles = 40, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
				.skip_border = {
					.wall_left_openings = {
						{ .start = 7, .size = 5, .active = 1, .target_room = 2, .target_x = 450, .target_y = 48 }
					},
					.wall_left_openings_count = 1,
				}
			}
		},
		.rooms = 3
	},
    [2] = {
        .tilename = "TLSTCSHW", .bgname = {"BGCSTHW1", "BGCSTHW2", "BGCSTHW3", "BGCSTHW4"},
        .enter_door = {.x = 28, .y = 111, .rotateu = 0, .open = 0,.active = 1},
        .exit_door = {.x = 984, .y = 111, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			[0] = {
				.width_tiles = 64, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
			}
		},
		.rooms = 1
	},
    [3] = {
        .tilename = "TLSTCAS", .bgname = {"BGCASTLE"},
        .enter_door = {.x = 28, .y = 272, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 272, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			[0] = {
				.width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL,
			}
		},
		.rooms = 1
	},
    [4] = {
        .tilename = "TLSTCLD", .bgname = {"BGCOLD"},
		.level_data = {
			[0] = {
				.width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL, .skip_border = {.skip_wall_left = 1, .skip_wall_rigth = 1, .skip_ceiling = 1}
			}
		},
		.rooms = 1
	},
    [5] = {
        .tilename = "TLSTGRN", .bgname = {"BGGROUND"},
		.exit_door = {.x = 442, .y = 80, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			[0] = {
				.width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL,
				.skip_border = {
					.skip_wall_left = 1, .skip_wall_rigth = 1, .skip_ceiling = 1,
					.floor_openings = {
						{ .start = 7, .size = 6, .active = 1 },
						{ .start = 20, .size = 6, .active = 1 }
					},
					.floor_openings_count = 2
				}
			},
            [1] = {
				.width_tiles = 30, .height_tiles = LEVEL_MAX_WIDTH, .level_orientation = LEVEL_VERTICAL,
				.skip_border = {
					.wall_left_openings = {
						{ .start = 120, .size = 5, .active = 1 }
					},
					.wall_left_openings_count = 1,
				}
			}
		},
		.rooms = LEVEL_06_ROOMS
	},
	[6] = {
        .tilename = "TLSTCAS", .bgname = {"BGCASTLE"},
        .enter_door = {.x = 28, .y = 272, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 272, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			[0] = {
				.width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL,
			}
		},
		.rooms = 1
	},
	[7] = {
        .tilename = "TLSTCAS", .bgname = {"BGCASTLE"},
        .enter_door = {.x = 28, .y = 272, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 272, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			[0] = {
				.width_tiles = 40, .height_tiles = LEVEL_MAX_HEIGHT, .level_orientation = LEVEL_HORIZONTAL
			}
		},
		.rooms = 1
	},
};

const LevelThemeTextures *level_manager_get_default_theme(void)
{
    return &s_default_theme;
}

DynamicLevel *level_manager_get_current_level(void)
{
    return &s_current_level;
}

void level_draw(uint8_t currentLevel, uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme) {
    
    switch (currentLevel)
    {
    case 1:
        level_01_builder(currentRoom, level, theme);
        break;
	case 2:
		level_02_builder(currentRoom, level, theme);
		break;
    case 6:
        level_06_builder(currentRoom, level, theme);
        break;
    default:
        break;
    }
}

void level_manager_render_entities(uint8_t currentLevel, uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr)
{
    if (currentRoom < 1 || currentRoom > LEVEL_01_ROOMS)
        return;

    LEVEL_DATA *room_data = &LEVELS_INFO[currentLevel - 1].level_data[currentRoom - 1];

    /* Obtener coordenadas y dimensiones de colisión de Many (espacio de mundo) */
    int32_t many_box_x = (int32_t)many_ptr->x;
    int32_t many_box_y = (int32_t)((many_ptr->y >> 8) + MANY_COLLISION_HEIGHT - 16);
    int32_t many_box_w = MANY_COLLISION_WIDTH;
    int32_t many_box_h = 16;

    /* Obtener hitbox del látigo de Many en este frame si está atacando */
    int32_t whip_x = 0, whip_y = 0, whip_w = 0, whip_h = 0;
    uint8_t whip_active = many_get_whip_hitbox(&whip_x, &whip_y, &whip_w, &whip_h);

    /* Renderizar y evaluar colisiones de los misceláneos de la sala */
    for (int16_t i = 0; i < ARRAY_TWENTY; i++)
    {
        MiscObject *misc = &room_data->miscObjects[i];
        if (!misc->active)
            continue;

        /* Si es reliquia (< MISC_CANDLE) */
        if (misc->type < MISC_CANDLE)
        {
            uint8_t relic_touched = relic_render_in_camera(
                misc, cam_x, cam_y,
                many_box_x, many_box_y, many_box_w, many_box_h);

            if (relic_touched)
            {
                many_collect_relic(many_ptr, misc->type);
            }
        }
        else
        {
            misc_update(misc, &s_current_level);

            /* Cualquier otro misc (velas, dinero, etc.) */
            int16_t hit_many = misc_render_in_camera(
                misc, cam_x, cam_y,
                many_box_x, many_box_y, many_box_w, many_box_h);

            int32_t misc_world_y = misc->y - misc->offset_y;

            /* Comprobar si el látigo de Many golpea la vela/antorcha */
            if (misc->type == MISC_CANDLE && whip_active && check_collision_box_int(whip_x, whip_y, whip_w, whip_h, misc->x, misc_world_y, 16, 16))
            {
                uint8_t money_value = drop_money();
                if (money_value)
                {
                    misc->type = MISC_CASH;
                    misc->horizontal_direction = 0;
                    misc->velocity_y = 0;
                    misc->falling = 1;
                    misc->value = money_value;
                    misc->r = money_value == 10 ? 111 : money_value == 50 ? 192 : 212;
                    misc->g = money_value == 10 ? 78 : money_value == 50 ? 192 : 175;
                    misc->b = money_value == 10 ? 55 : money_value == 50 ? 192 : 55;
                } else {
                    misc->active = 0;
                }
            } else if (misc->type != MISC_CANDLE && hit_many)
            {
                misc->active = 0;
                many_collect_misc(many_ptr, misc->type, misc->value);
            }
        }
    }

    /* 2. Actualizar, evaluar combate y renderizar los enemigos de la sala */
    int32_t many_full_y = (int32_t)(many_ptr->y >> 8);
    int32_t many_hurt_x = 0, many_hurt_y = 0, many_hurt_w = 0, many_hurt_h = 0;
    uint8_t many_can_take_damage = many_get_hurtbox(many_ptr, &many_hurt_x, &many_hurt_y, &many_hurt_w, &many_hurt_h);

    for (int16_t i = 0; i < MAX_ROOM_ENEMIES; i++)
    {
        Enemy *enemy = &room_data->enemies[i];
        if (!enemy->active)
            continue;

        /* Actualizar lógica e IA del enemigo */
        enemy_update(enemy, (int32_t)many_ptr->x, many_full_y, (int32_t)s_current_level.pixel_width, (int32_t)s_current_level.pixel_height);

        if (!enemy->active)
            continue;

        /* Comprobar si el látigo de Many golpea al enemigo */
        if (whip_active && check_collision_box_int(whip_x, whip_y, whip_w, whip_h,
                                                   enemy->x, enemy->y, enemy->col_w, enemy->col_h))
        {
            uint8_t killed = enemy_take_damage(enemy, 1);
            if (killed)
            {
                /* Otorga experiencia a Many */
                // TODO: many_add_exp(many_ptr, enemy->exp);
                continue;
            }
        }

        /* Comprobar colisión del enemigo contra el hurtbox de Many para infligir daño */
        if (enemy->active && many_can_take_damage &&
            check_collision_box_int(many_hurt_x, many_hurt_y, many_hurt_w, many_hurt_h,
                                    enemy->x, enemy->y, enemy->col_w, enemy->col_h))
        {
            many_take_damage(many_ptr, enemy->damage, enemy->x);
            /* Al entrar en estado HURT, queda invulnerable para evitar multihits en este mismo frame */
            many_can_take_damage = 0;
        }

        /* Renderizar enemigo en pantalla */
        enemy_render(enemy, cam_x, cam_y);
    }
}

int level_manager_load_room(uint8_t currentLevel, uint8_t currentRoom)
{
    if (currentLevel < 1 || currentLevel > LEVELS_COUNT)
        return -1;

    LEVEL_WORLD *level_info = &LEVELS_INFO[currentLevel - 1];
    if (currentRoom < 1 || currentRoom > level_info->rooms)
        return -1;

    LEVEL_DATA *room_data = &level_info->level_data[currentRoom - 1];

    for (uint8_t i = 0; i < ARRAY_TWENTY; i++)
    {
        MiscObject *misc = &room_data->miscObjects[i];

        if (misc->initial_state_saved || misc->type >= MISC_CANDLE)
        {
            if (!misc->initial_state_saved)
            {
                misc->initial_type = misc->type;
                misc->initial_y = misc->y;
                misc->initial_state_saved = 1;
                misc->initial_r = 128;
                misc->initial_g = 128;
                misc->initial_b = 128;
            }

            misc->type = misc->initial_type;
            misc->y = misc->initial_y;
            misc->active = 1;
            misc->velocity_y = 0;
            misc->falling = 0;
            misc->current_frame = 0;
            misc->offset_y = 0;
            misc->horizontal_direction = 0;
            misc->r = misc->initial_r;
            misc->g = misc->initial_g;
            misc->b = misc->initial_b;
        }

        /* Las reliquias mantienen su estado; los demás misc reaparecen al entrar. */
        if (misc->type >= MISC_CANDLE)
        {
            misc->active = 1;
            if (misc->type == MISC_CASH)
            {
                misc->type = MISC_CANDLE;
                // TODO Restablecer a la posicion original de la vela
                // misc->y -= 16;
            }
        }
    }

    /* Restablecer enemigos de la habitación a sus posiciones iniciales */
    for (uint8_t i = 0; i < MAX_ROOM_ENEMIES; i++)
    {
        Enemy *enemy = &room_data->enemies[i];
        if (enemy->type != ENEMY_NONE)
        {
            if (enemy->initial_x == 0 && enemy->initial_y == 0 && (enemy->x != 0 || enemy->y != 0))
            {
                enemy->initial_x = enemy->x;
                enemy->initial_y = enemy->y;
            }
            enemy_reset(enemy);
        }
    }

    char buffer[32];

    /* 1. Cargar texturas del nivel (tileset y fondos) desde CD solo si cambió el nivel */
    if (s_cached_level != currentLevel)
    {
        /* Cargar tileset si difiere del actual en VRAM */
        if (strcmp(s_cached_tilename, level_info->tilename) != 0)
        {
            sprintf(buffer, "\\%s.TIM", level_info->tilename);
            int res = cd_load_image(buffer, &s_tileset_tim);
            if (res != 0)
            {
                printf("Error al cargar tileset %s (codigo %d)\n", buffer, res);
                return res;
            }
            strncpy(s_cached_tilename, level_info->tilename, sizeof(s_cached_tilename) - 1);
            s_cached_tilename[sizeof(s_cached_tilename) - 1] = '\0';
        }

        /* Cargar fondos del nivel */
        s_background_count = 0;
        for (int i = 0; i < BACKGROUND_MAX; i++)
        {
            if (level_info->bgname[i][0] == '\0')
                continue;

            sprintf(buffer, "\\%s.TIM", level_info->bgname[i]);
            int result = cd_load_image(buffer, &s_background_tim[s_background_count]);
            if (result != 0)
            {
                printf("Error al cargar fondo %s (codigo %d)\n", buffer, result);
                return result;
            }
            s_background_count++;
        }

        s_cached_level = currentLevel;
    }

    /* 2. Generar base del nivel dinámico */
    level_generate_base(
        &s_current_level,
        room_data->width_tiles,
        room_data->height_tiles,
        room_data->level_orientation,
        &s_tileset_tim,
        &s_default_theme,
        room_data->skip_border);

    /* 3. Dibujar la geometría y plataformas específicas del nivel */
    level_draw(currentLevel, currentRoom, &s_current_level, &s_default_theme);

    /* 4. Asignar los fondos precargados en VRAM sin leer nuevamente el CD */
    CD_TEXTURE selected_backgrounds[BACKGROUND_MAX] = {0};
    uint8_t selected_count = room_data->background_count;

    if (selected_count == 0)
    {
        /* Compatibilidad: si no se configura la habitación, usar todos los fondos. */
        level_set_background(&s_current_level, s_background_tim, s_background_count);
    }
    else
    {
        if (selected_count > BACKGROUND_MAX || selected_count > s_background_count)
        {
            printf("Error: configuracion de fondos invalida en nivel %u, habitacion %u\n",
                currentLevel, currentRoom);
            return -1;
        }

        for (uint8_t i = 0; i < selected_count; i++)
        {
            uint8_t bg_index = room_data->background_indices[i];

            if (bg_index >= s_background_count)
            {
                printf("Error: indice de fondo invalido (%u) en nivel %u, habitacion %u\n",
                    bg_index, currentLevel, currentRoom);
                return -1;
            }

            selected_backgrounds[i] = s_background_tim[bg_index];
        }

        level_set_background(&s_current_level, selected_backgrounds, selected_count);
    }

    /* 5. Configurar puertas de entrada y salida de los extremos del nivel */
    if (level_info->enter_door.active && currentRoom == 1)
    {
        level_set_enter_door(&s_current_level,
                             level_info->enter_door.x,
                             level_info->enter_door.y,
                             level_info->enter_door.rotateu,
                             level_info->enter_door.open);
    }
    if (level_info->exit_door.active && currentRoom == level_info->rooms)
    {
        level_set_exit_door(&s_current_level,
                            level_info->exit_door.x,
                            level_info->exit_door.y,
                            level_info->exit_door.rotateu,
                            level_info->exit_door.open);
    }

    return 0;
}

void level_manager_spawn_player(uint8_t currentLevel, uint8_t currentRoom,
                                uint8_t spawn_at_exit,
                                int32_t custom_spawn_x, int32_t custom_spawn_y)
{
    (void)currentLevel;
    (void)currentRoom;

    MANY_OBJECT *many_ptr = many_get_instance();
    const int32_t spawn_margin = 2 * TILE_SIZE;
    int32_t default_spawn_y = (int32_t)s_current_level.pixel_height - (3 * TILE_SIZE) - MANY_COLLISION_HEIGHT;

    int32_t left_spawn_x = spawn_margin;
    int32_t right_spawn_x = (int32_t)s_current_level.pixel_width - MANY_COLLISION_WIDTH - spawn_margin;
    LevelDoor *exit_door = level_get_exit_door(&s_current_level);
    LevelDoor *enter_door = level_get_enter_door(&s_current_level);

    if (custom_spawn_x >= 0 || custom_spawn_y >= 0)
    {
        int32_t spawn_x = (custom_spawn_x >= 0)
                              ? custom_spawn_x
                              : (spawn_at_exit ? right_spawn_x : left_spawn_x);
        int32_t spawn_y = (custom_spawn_y >= 0)
                              ? custom_spawn_y
                              : default_spawn_y;

        many_set_position(spawn_x, spawn_y);
        many_ptr->direction = spawn_at_exit ? 0 : 1;
    }
    else if (spawn_at_exit)
    {
        int32_t spawn_x = (exit_door && exit_door->active)
                              ? (exit_door->x - MANY_COLLISION_WIDTH - 8)
                              : right_spawn_x;
        many_set_position(spawn_x, default_spawn_y);
        many_ptr->direction = 0; // Mirando a la izquierda al entrar desde la salida
    }
    else
    {
        int32_t spawn_x = (enter_door && enter_door->active)
                              ? (enter_door->x + DOOR_W_BASE + 8)
                              : left_spawn_x;
        many_set_position(spawn_x, default_spawn_y);
        many_ptr->direction = 1; // Mirando a la derecha al entrar desde la entrada
    }
}

LevelTransitionResult level_manager_check_transitions(uint8_t currentLevel, uint8_t currentRoom)
{
    LevelTransitionResult result;
    memset(&result, 0, sizeof(result));
    result.custom_spawn_x = -1;
    result.custom_spawn_y = -1;
    result.transition_door_idx = -1;
    result.target_level = currentLevel;
    result.target_room = currentRoom;

    if (currentLevel < 1 || currentLevel > LEVELS_COUNT)
        return result;

    LEVEL_WORLD *level_info = &LEVELS_INFO[currentLevel - 1];
    MANY_OBJECT *many_ptr = many_get_instance();

    /* Bounding box de los pies de Many (enteros sin floats) */
    int32_t many_box_x = many_ptr->x;
    int32_t many_box_y = (many_ptr->y >> 8) + MANY_COLLISION_HEIGHT - 16;
    int32_t many_box_w = MANY_COLLISION_WIDTH;
    int32_t many_box_h = 16;

    /* 1. Comprobar colisión con puertas activas */
    for (uint8_t d = 0; d < s_current_level.door_count; d++)
    {
        LevelDoor *door = &s_current_level.doors[d];
        if (!door->active)
            continue;

        if (door->type == DOOR_TYPE_ENTER && currentRoom == 1 && currentLevel > 1)
        {
            if (check_collision_box_int(many_box_x, many_box_y, many_box_w, many_box_h,
                                        door->x - 16, door->y + DOOR_H - 16, DOOR_W_BASE + 16, 16))
            {
                door->open = 1;
                result.triggered = 1;
                result.door_transition = TRANSITION_ENTER_DOOR;
                result.transition_door_idx = (int8_t)d;
                result.target_level = currentLevel - 1;
                result.target_room = LEVELS_INFO[currentLevel - 2].rooms;
                result.spawn_at_exit = 1;
                return result;
            }
        }
        else if (door->type == DOOR_TYPE_EXIT && currentRoom == level_info->rooms && currentLevel < LEVELS_COUNT)
        {
            if (check_collision_box_int(many_box_x, many_box_y, many_box_w, many_box_h,
                                        door->x, door->y + DOOR_H - 16, DOOR_W_BASE + 16, 16))
            {
                door->open = 1;
                result.triggered = 1;
                result.door_transition = TRANSITION_EXIT_DOOR;
                result.transition_door_idx = (int8_t)d;
                result.target_level = currentLevel + 1;
                result.target_room = 1;
                result.spawn_at_exit = 0;
                return result;
            }
        }
    }

    /* 2. Comprobar bordes de cuarto y aberturas/huecos configurados */
    LevelDoor *active_enter_door = level_get_enter_door(&s_current_level);
    LevelDoor *active_exit_door = level_get_exit_door(&s_current_level);
    const LevelSkipBorder *current_border = &level_info->level_data[currentRoom - 1].skip_border;

    /* Borde Izquierdo */
    if (many_ptr->x <= 0)
    {
        int32_t many_mid_y = (many_ptr->y >> 8) + (MANY_COLLISION_HEIGHT / 2);
        uint16_t many_ty = (uint16_t)(many_mid_y / TILE_SIZE);
        const BorderOpening *opening = level_border_find_opening_at(
            current_border->wall_left_openings,
            current_border->wall_left_openings_count,
            many_ty);

        if (opening && opening->target_room > 0)
        {
            result.triggered = 1;
            result.target_room = opening->target_room;
            if (opening->target_level > 0)
                result.target_level = opening->target_level;
            result.spawn_at_exit = 1;
            result.custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : -1;
            result.custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : -1;
            return result;
        }
        else if (currentRoom > 1)
        {
            result.triggered = 1;
            result.target_room = currentRoom - 1;
            result.spawn_at_exit = 1;
            return result;
        }
        else if (currentLevel > 1 && (!active_enter_door || !active_enter_door->active))
        {
            result.triggered = 1;
            result.target_level = currentLevel - 1;
            result.target_room = LEVELS_INFO[currentLevel - 2].rooms;
            result.spawn_at_exit = 1;
            return result;
        }
    }

    /* Borde Derecho */
    int32_t room_max_x = (int32_t)s_current_level.pixel_width - MANY_COLLISION_WIDTH;
    if ((int32_t)many_ptr->x >= room_max_x)
    {
        int32_t many_mid_y = (many_ptr->y >> 8) + (MANY_COLLISION_HEIGHT / 2);
        uint16_t many_ty = (uint16_t)(many_mid_y / TILE_SIZE);
        const BorderOpening *opening = level_border_find_opening_at(
            current_border->wall_right_openings,
            current_border->wall_right_openings_count,
            many_ty);

        if (opening && opening->target_room > 0)
        {
            result.triggered = 1;
            result.target_room = opening->target_room;
            if (opening->target_level > 0)
                result.target_level = opening->target_level;
            result.spawn_at_exit = 0;
            result.custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : -1;
            result.custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : -1;
            return result;
        }
        else if (currentRoom < level_info->rooms)
        {
            result.triggered = 1;
            result.target_room = currentRoom + 1;
            result.spawn_at_exit = 0;
            return result;
        }
        else if (currentLevel < LEVELS_COUNT && (!active_exit_door || !active_exit_door->active))
        {
            result.triggered = 1;
            result.target_level = currentLevel + 1;
            result.target_room = 1;
            result.spawn_at_exit = 0;
            return result;
        }
    }

    /* Borde Superior (Techo) */
    if ((many_ptr->y >> 8) <= 0)
    {
        int32_t many_mid_x = (int32_t)many_ptr->x + (MANY_COLLISION_WIDTH / 2);
        uint16_t many_tx = (uint16_t)(many_mid_x / TILE_SIZE);
        const BorderOpening *opening = level_border_find_opening_at(
            current_border->ceiling_openings,
            current_border->ceiling_openings_count,
            many_tx);

        if (opening && opening->target_room > 0)
        {
            result.triggered = 1;
            result.target_room = opening->target_room;
            if (opening->target_level > 0)
                result.target_level = opening->target_level;
            result.custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : many_ptr->x;
            result.custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : -1;
            return result;
        }
    }

    /* Borde Inferior (Piso) */
    int32_t room_max_y = (int32_t)s_current_level.pixel_height - MANY_COLLISION_HEIGHT;
    if ((many_ptr->y >> 8) >= room_max_y)
    {
        int32_t many_mid_x = (int32_t)many_ptr->x + (MANY_COLLISION_WIDTH / 2);
        uint16_t many_tx = (uint16_t)(many_mid_x / TILE_SIZE);
        const BorderOpening *opening = level_border_find_opening_at(
            current_border->floor_openings,
            current_border->floor_openings_count,
            many_tx);

        if (opening && opening->target_room > 0)
        {
            result.triggered = 1;
            result.target_room = opening->target_room;
            if (opening->target_level > 0)
                result.target_level = opening->target_level;
            result.custom_spawn_x = (opening->target_x >= 0) ? opening->target_x : many_ptr->x;
            result.custom_spawn_y = (opening->target_y >= 0) ? opening->target_y : 1;
            return result;
        }
    }

    return result;
}

void level_manager_play_door_transition(const LevelTransitionResult *transition_info)
{
    if (!transition_info || transition_info->door_transition == TRANSITION_NONE)
        return;

    LevelDoor *door = (transition_info->transition_door_idx >= 0)
                          ? level_get_door(&s_current_level, (uint8_t)transition_info->transition_door_idx)
                          : ((transition_info->door_transition == TRANSITION_EXIT_DOOR)
                                 ? level_get_exit_door(&s_current_level)
                                 : level_get_enter_door(&s_current_level));

    /* 1. Esperar a que la puerta termine de abrirse */
    while (door && door->current_frame < DOOR_SPRITES - 1)
    {
        MANY_OBJECT *many_ptr = many_get_instance();
        level_update_camera(&s_current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

        int32_t cam_x, cam_y;
        level_get_camera_pos(&s_current_level, &cam_x, &cam_y);

        level_render(&s_current_level);
        many_render_in_camera(cam_x, cam_y);
        graphic_display();
    }

    /* 2. Many corre y cruza automáticamente la puerta */
    if (transition_info->door_transition == TRANSITION_EXIT_DOOR)
    {
        int32_t target_x = door ? (door->x + 32) : ((int32_t)s_current_level.pixel_width);
        MANY_OBJECT *many_ptr = many_get_instance();
        while ((int32_t)many_ptr->x < target_x)
        {
            many_auto_walk_step(1);

            level_update_camera(&s_current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

            int32_t cam_x, cam_y;
            level_get_camera_pos(&s_current_level, &cam_x, &cam_y);

            level_render(&s_current_level);
            many_render_in_camera(cam_x, cam_y);
            graphic_display();
        }
    }
    else if (transition_info->door_transition == TRANSITION_ENTER_DOOR)
    {
        int32_t target_x = door ? (door->x - 32) : 0;
        MANY_OBJECT *many_ptr = many_get_instance();
        while ((int32_t)many_ptr->x > target_x)
        {
            many_auto_walk_step(-1);

            level_update_camera(&s_current_level, many_ptr->x, (many_ptr->y >> 8), MANY_COLLISION_WIDTH, MANY_COLLISION_HEIGHT);

            int32_t cam_x, cam_y;
            level_get_camera_pos(&s_current_level, &cam_x, &cam_y);

            level_render(&s_current_level);
            many_render_in_camera(cam_x, cam_y);
            graphic_display();
        }
    }
}
