#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../../engine/cd_loader.h"

#define TILE_SIZE 16

/* Limites seguros de tamano de mapa en tiles para la PS1 (RAM 2MB)
 * 128 x 64 tiles = 8,192 bytes (~8 KB de RAM por nivel),
 * equivale a 2048 x 1024 pixeles (6.4 pantallas de ancho x 4.2 pantallas de alto).
 */
#define LEVEL_MAX_WIDTH 128
#define LEVEL_MAX_HEIGHT 64

#define BACKGROUND_MAX 4
#define LEVEL_MAX_DOORS 8
#define LEVEL_MAX_OPENINGS 4

typedef enum
{
    LEVEL_VERTICAL,
    LEVEL_HORIZONTAL
} LEVEL_ORIENTATION;

/* Tipos de colision y comportamiento de cada celda del tilemap */
typedef enum
{
    TILE_COLL_NONE = 0,  /* Espacio libre / fondo sin colision */
    TILE_COLL_SOLID = 1, /* Solido completo: bloquea piso, pared y techo */
    TILE_COLL_ONEWAY = 2 /* Plataforma atravesable desde abajo (semi-solida) */
} TileCollisionType;

/* Definicion de textura / coordenadas UV en el tileset.tim (16x16 cada tile) */
typedef struct
{
    uint8_t u;
    uint8_t v;
    TileCollisionType collision;
} TileTextureDef;

/* Estructura para configurar texturas del generador */
typedef struct
{
    TileTextureDef floor_tile;   /* Textura para pisos */
    TileTextureDef filled_tile;  /* Textura para relleno */
    TileTextureDef wall_tile;    /* Textura para paredes laterales */
    TileTextureDef ceiling_tile; /* Textura para techo */
    TileTextureDef pltfrml_tile; /* Textura para plataformas borde izquierdo */
    TileTextureDef pltfrmf_tile; /* Textura para plataformas relleno */
    TileTextureDef pltfrmr_tile; /* Textura para plataformas borde derecho */
    TileTextureDef bg_tile;      /* Textura opcional para fondo (o aire) */
    uint8_t use_bg_tile; /* 1 para rellenar el fondo con bg_tile */
} LevelThemeTextures;

/* Datos de cada celda del nivel */
typedef struct
{
    uint8_t u;              /* Coord U en tileset.tim */
    uint8_t v;              /* Coord V en tileset.tim */
    uint8_t collision_type; /* TileCollisionType: NONE, SOLID, ONEWAY */
    uint8_t active;         /* 1 si tiene tile renderizable, 0 si aire vacio */
} LevelTile;

/* Camara 2D con scroll y limites */
typedef struct
{
    int32_t x;          /* Posicion X en pixeles del mundo */
    int32_t y;          /* Posicion Y en pixeles del mundo */
    int32_t viewport_w; /* Ancho de pantalla (320) */
    int32_t viewport_h; /* Alto de pantalla (240) */
} LevelCamera;

/* Hueco / abertura configurable en un borde del nivel */
typedef struct
{
    uint16_t start;       /* Tile inicial del hueco (coordenada de tile en el eje correspondiente) */
    uint16_t size;        /* Tamano del hueco en tiles */
    uint8_t active;       /* 1 si la abertura esta habilitada, 0 si no */
    uint8_t target_room;  /* 0 = automatico (room -/+ 1), >0 = habitacion especifica (1-based) */
    uint8_t target_level; /* 0 = mismo nivel, >0 = nivel especifico (1-based) */
    int16_t target_x;     /* Posicion X en pixeles en destino (-1 = automatico segun direccion) */
    int16_t target_y;     /* Posicion Y en pixeles en destino (-1 = automatico segun piso) */
} BorderOpening;

/* Configuracion avanzada de bordes con soporte para omitir bordes completos
 * o definir huecos/aberturas de tamano y posicion personalizada.
 */
typedef struct
{
    uint8_t skip_floor;
    uint8_t skip_ceiling;
    uint8_t skip_wall_left;
    uint8_t skip_wall_rigth; /* Se preserva el nombre original para compatibilidad */

    /* Huecos / aberturas personalizadas en cada borde (tamano y posicion en tiles) */
    BorderOpening floor_openings[LEVEL_MAX_OPENINGS];
    uint8_t floor_openings_count;

    BorderOpening ceiling_openings[LEVEL_MAX_OPENINGS];
    uint8_t ceiling_openings_count;

    BorderOpening wall_left_openings[LEVEL_MAX_OPENINGS];
    uint8_t wall_left_openings_count;

    BorderOpening wall_right_openings[LEVEL_MAX_OPENINGS];
    uint8_t wall_right_openings_count;
} LevelSkipBorder;

typedef LevelSkipBorder LevelBorderConfig;

/* Tipos de puertas en el nivel */
typedef enum
{
    DOOR_TYPE_GENERIC = 0,
    DOOR_TYPE_ENTER   = 1,
    DOOR_TYPE_EXIT    = 2
} LevelDoorType;

/* Estructura para configurar una puerta en el nivel */
typedef struct
{
    int32_t x;             /* Posicion X en pixeles del mundo */
    int32_t y;             /* Posicion Y en pixeles del mundo */
    uint8_t rotateu;       /* 0: orientacion normal, 1: volteada horizontalmente */
    uint8_t open;          /* 0: cerrada, 1: abierta */
    uint8_t active;        /* 1 si la puerta esta configurada/activa, 0 si no */
    uint8_t current_frame; /* Frame actual de animacion de la puerta */
    uint8_t timer;         /* Timer para avance de frame */
    uint8_t type;          /* LevelDoorType (DOOR_TYPE_GENERIC, DOOR_TYPE_ENTER, DOOR_TYPE_EXIT) */
    int16_t target_room;   /* Habitacion de destino (-1 para no configurada o segun logica global) */
    int16_t target_level;  /* Nivel de destino (-1 para no configurado o segun logica global) */
} LevelDoor;

/* Estructura principal del nivel dinamico */
typedef struct
{
    uint16_t width;                        /* Ancho del nivel en tiles */
    uint16_t height;                       /* Alto del nivel en tiles */
    uint32_t pixel_width;                  /* Ancho en pixeles (width * TILE_SIZE) */
    uint32_t pixel_height;                 /* Alto en pixeles (height * TILE_SIZE) */
    LEVEL_ORIENTATION level_orientation;
    CD_TEXTURE *tileset;                   /* Puntero a la textura del tileset */
    CD_TEXTURE background[BACKGROUND_MAX]; /* Texturas para fondos */
    uint8_t background_count;
    TileTextureDef bg_tile;
    uint8_t use_bg_tile;
    LevelCamera camera;                                 /* Camara asociada al nivel */
    LevelTile tiles[LEVEL_MAX_HEIGHT][LEVEL_MAX_WIDTH]; /* Celdas del nivel */

    /* Coleccion de puertas del nivel dinamico */
    LevelDoor doors[LEVEL_MAX_DOORS];
    uint8_t door_count;
} DynamicLevel;

/* ========================================================================= */
/* FUNCIONES DE CREACION Y CONFIGURACION DEL NIVEL                          */
/* ========================================================================= */

/**
 * Inicializa un nivel dinamico con ancho y alto en tiles y la textura del tileset.
 * Si width o height exceden LEVEL_MAX_WIDTH/HEIGHT, se acotan al maximo seguro.
 */
void level_init(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles, LEVEL_ORIENTATION level_orientation, CD_TEXTURE *tileset_tim);

/**
 * Limpia todo el nivel dejandolo vacio (aire / sin colision).
 */
void level_clear(DynamicLevel *level);

/**
 * Establece un tile individual en coordenadas de tile (tx, ty).
 */
void level_set_tile(DynamicLevel *level, uint16_t tx, uint16_t ty, TileTextureDef tile_def);

void level_set_background(DynamicLevel *level,
                          CD_TEXTURE background_tim[BACKGROUND_MAX],
                          uint8_t count);

/**
 * Limpia todas las puertas registradas en el nivel.
 */
void level_clear_doors(DynamicLevel *level);

/**
 * Agrega una puerta al array de puertas del nivel.
 * Retorna el indice asignado o -1 si se alcanzo el maximo.
 */
int8_t level_add_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open, uint8_t type);

/**
 * Obtiene un puntero a una puerta por indice en el array.
 */
LevelDoor *level_get_door(DynamicLevel *level, uint8_t index);

/**
 * Obtiene la primera puerta que coincida con el tipo dado (ej. DOOR_TYPE_ENTER o DOOR_TYPE_EXIT).
 * Retorna NULL si no se encuentra ninguna activa.
 */
LevelDoor *level_get_door_by_type(DynamicLevel *level, uint8_t type);

/**
 * Helpers de conveniencia para obtener la puerta de entrada y de salida.
 */
LevelDoor *level_get_enter_door(DynamicLevel *level);
LevelDoor *level_get_exit_door(DynamicLevel *level);

/**
 * Configura la posicion y propiedades de la puerta de entrada (agrega o actualiza en el array).
 */
void level_set_enter_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open);

/**
 * Configura la posicion y propiedades de la puerta de salida (agrega o actualiza en el array).
 */
void level_set_exit_door(DynamicLevel *level, int32_t x, int32_t y, uint8_t rotateu, uint8_t open);

/**
 * Helpers para anadir huecos / aberturas personalizadas a los bordes.
 */
void level_border_add_floor_opening(LevelSkipBorder *border, uint16_t start_tx, uint16_t size_tx);
void level_border_add_ceiling_opening(LevelSkipBorder *border, uint16_t start_tx, uint16_t size_tx);
void level_border_add_wall_left_opening(LevelSkipBorder *border, uint16_t start_ty, uint16_t size_ty);
void level_border_add_wall_right_opening(LevelSkipBorder *border, uint16_t start_ty, uint16_t size_ty);

/**
 * Busca una abertura activa en una lista que contenga la posicion dada en tiles.
 * Retorna puntero a la abertura o NULL si ninguna coincide.
 */
const BorderOpening *level_border_find_opening_at(const BorderOpening *openings, uint8_t count, uint16_t pos);

/**
 * Genera el perimetro del nivel: piso, techo y paredes con sus respectivas texturas.
 * floor_thickness: grosor del piso en tiles (ej. 2 o 3)
 * ceiling_thickness: grosor del techo en tiles (ej. 1 o 2)
 * wall_thickness: grosor de paredes laterales en tiles (ej. 1 o 2)
 */
void level_build_border(DynamicLevel *level,
                        TileTextureDef floor_tile, uint8_t floor_thickness,
                        TileTextureDef ceiling_tile, uint8_t ceiling_thickness,
                        TileTextureDef wall_tile, uint8_t wall_thickness,
                        TileTextureDef filled_tile,
                        LevelSkipBorder skip_border);

/**
 * Crea una plataforma horizontal en coordenadas de tile (tx, ty) de longitud 'length_tiles'.
 * is_oneway: true si es atravesable desde abajo (semi-solida), false si es bloque solido.
 */
void level_add_platform(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t length_tiles,
                        TileTextureDef pltfrml_tile,
                        TileTextureDef pltfrmf_tile,
                        TileTextureDef pltfrmr_tile,
                        bool is_oneway);

/**
 * Crea una pared vertical o columna en coordenadas de tile (tx, ty) de altura 'height_tiles'.
 */
void level_add_wall(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t height_tiles,
                    TileTextureDef wall_tile);

/**
 * Rellena un area rectangular de tiles con una definicion de textura/colision.
 */
void level_fill_rect(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t w_tiles, uint16_t h_tiles,
                     TileTextureDef tile_def);

/**
 * Generador procedural/estilo plantilla: genera la base del nivel.
 */
void level_generate_base(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles, LEVEL_ORIENTATION level_orientation,
                         CD_TEXTURE *tileset_tim, const LevelThemeTextures *theme, LevelSkipBorder skip_border);

/* ========================================================================= */
/* FUNCIONES DE CAMARA Y SCROLL                                              */
/* ========================================================================= */

/**
 * Actualiza la camara para seguir a una entidad (ej. Many) centrandola en pantalla
 * y respetando estrictamente los limites del nivel en X e Y.
 * target_x, target_y: posicion mundial de la entidad (en pixeles).
 * target_w, target_h: dimensiones de la entidad.
 */
void level_update_camera(DynamicLevel *level, int32_t target_x, int32_t target_y, int32_t target_w, int32_t target_h);

/**
 * Obtiene la posicion actual de la camara.
 */
void level_get_camera_pos(const DynamicLevel *level, int32_t *out_cam_x, int32_t *out_cam_y);

/**
 * Establece directamente la posicion de la camara respetando los limites del nivel.
 */
void level_set_camera_pos(DynamicLevel *level, int32_t cam_x, int32_t cam_y);

/* ========================================================================= */
/* RENDERIZADO OPTIMIZADO (FRUSTUM CULLING)                                  */
/* ========================================================================= */

/**
 * Renderiza unicamente los tiles visibles dentro del viewport de la camara,
 * transformando coordenadas de mundo a coordenadas de pantalla (world_x - cam_x).
 */
void level_render(const DynamicLevel *level);

/* ========================================================================= */
/* DETECCION Y RESOLUCION DE COLISIONES                                      */
/* ========================================================================= */

/**
 * Obtiene el tipo de colision en un tile especifico.
 */
TileCollisionType level_get_tile_collision(const DynamicLevel *level, int32_t tx, int32_t ty);

/**
 * Verifica si un punto en el mundo choca con un tile solido.
 */
bool level_point_is_solid(const DynamicLevel *level, int32_t world_x, int32_t world_y);

/**
 * Verifica si un rectangulo AABB (en coordenadas de mundo) colisiona con cualquier
 * solido del nivel o se sale de los limites del nivel.
 */
bool level_check_box_collision(const DynamicLevel *level, int32_t x, int32_t y, int32_t w, int32_t h);

/**
 * Resuelve el movimiento y las colisiones de un personaje con el nivel:
 * - Aplica y clamp los limites del nivel (izq, der, arriba, abajo).
 * - Detecta y bloquea choques con paredes laterales.
 * - Detecta y bloquea choques con techo.
 * - Detecta aterrizaje en piso o plataformas semi-solidas (one-way).
 *
 * *io_x, *io_y: posicion del personaje en pixeles (entrada y salida corregida).
 * *io_vx, *io_vy: velocidad del personaje (puesta a 0 al colisionar).
 * w, h: caja de colision del personaje.
 * is_grounded: salida booleana que indica si el personaje quedo pisando suelo/plataforma.
 */
void level_resolve_entity_collision(const DynamicLevel *level,
                                    int32_t *io_x, int32_t *io_y,
                                    int32_t *io_vx, int32_t *io_vy,
                                    int32_t w, int32_t h,
                                    int32_t ignored_oneway_ty,
                                    bool *is_grounded);
