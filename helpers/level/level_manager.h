#pragma once

#include "level_generator.h"
#include "../../gameplay/many.h"
#include "../../gameplay/misc.h"
#include "../../graphics/door.h"

#define LEVELS_COUNT 8
#define LEVEL_MAX_ROOMS 10
#define ARRAY_TWENTY 20

typedef struct
{
    uint16_t width_tiles, height_tiles;
    LevelSkipBorder skip_border;
    LEVEL_ORIENTATION level_orientation;
    uint8_t background_indices[BACKGROUND_MAX]; /* Indices de bgname, empezando en 0 */
    uint8_t background_count;                   /* 0 = usar todos los fondos del nivel */
    MiscObject miscObjects[ARRAY_TWENTY];
} LEVEL_DATA;

typedef struct
{
    char tilename[ARRAY_TWENTY];
    char bgname[BACKGROUND_MAX][ARRAY_TWENTY];
    LevelDoor enter_door;
    LevelDoor exit_door;
    LEVEL_DATA level_data[LEVEL_MAX_ROOMS];
    uint8_t rooms; // Max 10
} LEVEL_WORLD;

/* Resultado de evaluar una transición (puerta o borde de pantalla) */
typedef struct
{
    uint8_t triggered;          /* 1 si se activó cambio de cuarto o nivel */
    uint8_t target_level;       /* Nivel de destino (1..LEVELS_COUNT) */
    uint8_t target_room;        /* Cuarto de destino (1..rooms) */
    uint8_t spawn_at_exit;      /* 1 si Many debe aparecer del lado de salida, 0 si del lado de entrada */
    int32_t custom_spawn_x;     /* Coordenada X forzada (-1 si no aplica) */
    int32_t custom_spawn_y;     /* Coordenada Y forzada (-1 si no aplica) */
    DoorTransitionType door_transition; /* TRANSITION_ENTER_DOOR, TRANSITION_EXIT_DOOR o TRANSITION_NONE */
    int8_t transition_door_idx; /* Índice de la puerta involucrada en la transición */
} LevelTransitionResult;

extern LEVEL_WORLD LEVELS_INFO[LEVELS_COUNT];

/* Retorna el tema de texturas estático por defecto para niveles */
const LevelThemeTextures *level_manager_get_default_theme(void);

/* Retorna el puntero único estático al nivel actual (evita reservar ~8.5KB en el stack) */
DynamicLevel *level_manager_get_current_level(void);

/* Carga el tileset (con cache VRAM), fondos y construye el cuarto actual */
int level_manager_load_room(uint8_t currentLevel, uint8_t currentRoom);

/* Posiciona a Many según de dónde viene (inicio, nivel anterior/siguiente o hueco personalizado) */
void level_manager_spawn_player(uint8_t currentLevel, uint8_t currentRoom,
                                uint8_t spawn_at_exit,
                                int32_t custom_spawn_x, int32_t custom_spawn_y);

/* Evalúa colisiones de Many con puertas y bordes de sala utilizando aritmética entera (sin floats) */
LevelTransitionResult level_manager_check_transitions(uint8_t currentLevel, uint8_t currentRoom);

/* Ejecuta la animación y desplazamiento automático de Many al cruzar una puerta */
void level_manager_play_door_transition(const LevelTransitionResult *transition_info);

void level_draw(uint8_t currentLevel, uint8_t currentRoom, DynamicLevel *level, const LevelThemeTextures *theme);
/* Renderiza las entidades específicas de la sala actual (misceláneos, reliquias, enemigos y combate) */
void level_manager_render_entities(uint8_t currentLevel, uint8_t currentRoom, int32_t cam_x, int32_t cam_y, MANY_OBJECT *many_ptr);
