#include "level_manager.h"

#include "../../world/level01.h"
#include "../../world/level06.h"

const LEVEL_WORLD LEVELS_INFO[LEVELS_COUNT] = {
    {
		.tilename = "TLSTEXT",
        .bgname = {"BGEXT"},
		.level_data = {
			{ .width_tiles = 40, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL, .skip_border = {.skip_wall_left = 1, .skip_wall_rigth = 1, .skip_ceiling = 1}, },
			{
				.width_tiles = 40, .height_tiles = 50, .level_orientation = LEVEL_HORIZONTAL,
				.skip_border = {
					.skip_ceiling = 1,
					.wall_left_openings = {
						{ .start = 0, .size = 5, .active = 1 }
					},
					.wall_left_openings_count = 1,
					.wall_right_openings = {
						{ .start = 0, .size = 5, .active = 1},
						{ .start = 42, .size = 5, .active = 1}
					},
					.wall_right_openings_count = 2
				}
			},
			{
				.width_tiles = 40, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL,
				.skip_border = {
					.skip_wall_left = 1,
					.skip_ceiling = 1,
					.wall_right_openings = {
						{ .start = 7, .size = 5, .active = 1 }
					},
					.wall_right_openings_count = 1
				},
			},
		},
		.rooms = LEVEL_01_ROOMS
	},
    {
		.tilename = "TLSTENT",
        .bgname = {"BGENT1", "BGENT2", "BGENT3", "BGENT4"},
        .enter_door = {.x = 28, .y = 111, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 2008, .y = 111, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			{ .width_tiles = 128, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL }
		},
		.rooms = 1
	},
    {
		.tilename = "TLSTCSHW",
        .bgname = {"BGCSTHW1", "BGCSTHW2", "BGCSTHW3", "BGCSTHW4"},
        .enter_door = {.x = 28, .y = 111, .rotateu = 0, .open = 0,.active = 1},
        .exit_door = {.x = 984, .y = 111, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			{ .width_tiles = 64, .height_tiles = 15, .level_orientation = LEVEL_HORIZONTAL }
		},
		.rooms = 1
	},
    {
		.tilename = "TLSTCAS",
        .bgname = {"BGCASTLE"},
        .enter_door = {.x = 28, .y = 272, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 272, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			{ .width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL }
		},
		.rooms = 1
	},
    {
		.tilename = "TLSTCLD",
        .bgname = {"BGCOLD"},
		.level_data = {
			{ .width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL, .skip_border = {.skip_wall_left = 1, .skip_wall_rigth = 1, .skip_ceiling = 1} }
		},
		.rooms = 1
	},
    {
		.tilename = "TLSTGRN",
        .bgname = {"BGGROUND"},
		.level_data = {
			{ .width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL, .skip_border = {.skip_wall_left = 1, .skip_wall_rigth = 1, .skip_ceiling = 1, .skip_floor = 1} },
            { .width_tiles = 30, .height_tiles = LEVEL_MAX_WIDTH, .level_orientation = LEVEL_VERTICAL }
		},
		.rooms = LEVEL_06_ROOMS
	},
	{
		.tilename = "TLSTCAS",
        .bgname = {"BGCASTLE"},
        .enter_door = {.x = 28, .y = 272, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 272, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			{ .width_tiles = 40, .height_tiles = 25, .level_orientation = LEVEL_HORIZONTAL }
		},
		.rooms = 1
	},
	{
		.tilename = "TLSTCAS",
        .bgname = {"BGCASTLE"},
        .enter_door = {.x = 28, .y = 272, .rotateu = 0, .open = 0, .active = 1},
        .exit_door = {.x = 600, .y = 272, .rotateu = 1, .open = 0, .active = 1},
		.level_data = {
			{ .width_tiles = 40, .height_tiles = LEVEL_MAX_HEIGHT, .level_orientation = LEVEL_HORIZONTAL }
		},
		.rooms = 1
	},
};

void level_draw(uint8_t currentLevel, uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme) {
    
    switch (currentLevel)
    {
    case 1:
        level_01_builder(currentRoom, level, theme);
        break;
    case 6:
        level_06_builder(currentRoom, level, theme);
        break;
    default:
        break;
    }
}