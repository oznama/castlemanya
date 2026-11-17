#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../engine/cd_image.h"

#define TILE_SIZE 16

/* Limites seguros de tamano de mapa en tiles para la PS1 (RAM 2MB)
 * 128 x 64 tiles = 8,192 bytes (~8 KB de RAM por nivel),
 * equivale a 2048 x 1024 pixeles (6.4 pantallas de ancho x 4.2 pantallas de alto).
 */
#define LEVEL_MAX_WIDTH 128
#define LEVEL_MAX_HEIGHT 64

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
    TileTextureDef floor_tile;    /* Textura para pisos */
    TileTextureDef wall_tile;     /* Textura para paredes laterales */
    TileTextureDef ceiling_tile;  /* Textura para techo */
    TileTextureDef platform_tile; /* Textura para plataformas */
    TileTextureDef bg_tile;       /* Textura opcional para fondo (o aire) */
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

/* Estructura principal del nivel dinamico */
typedef struct
{
    uint16_t width;                                     /* Ancho del nivel en tiles */
    uint16_t height;                                    /* Alto del nivel en tiles */
    uint32_t pixel_width;                               /* Ancho en pixeles (width * TILE_SIZE) */
    uint32_t pixel_height;                              /* Alto en pixeles (height * TILE_SIZE) */
    CD_TEXTURE *tileset;                                /* Puntero a la textura del tileset */
    LevelCamera camera;                                 /* Camara asociada al nivel */
    LevelTile tiles[LEVEL_MAX_HEIGHT][LEVEL_MAX_WIDTH]; /* Celdas del nivel */
} DynamicLevel;

/* ========================================================================= */
/* FUNCIONES DE CREACION Y CONFIGURACION DEL NIVEL                          */
/* ========================================================================= */

/**
 * Inicializa un nivel dinamico con ancho y alto en tiles y la textura del tileset.
 * Si width o height exceden LEVEL_MAX_WIDTH/HEIGHT, se acotan al maximo seguro.
 */
void level_init(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles, CD_TEXTURE *tileset_tim);

/**
 * Limpia todo el nivel dejandolo vacio (aire / sin colision).
 */
void level_clear(DynamicLevel *level);

/**
 * Establece un tile individual en coordenadas de tile (tx, ty).
 */
void level_set_tile(DynamicLevel *level, uint16_t tx, uint16_t ty, TileTextureDef tile_def);

/**
 * Genera el perimetro del nivel: piso, techo y paredes con sus respectivas texturas.
 * floor_thickness: grosor del piso en tiles (ej. 2 o 3)
 * ceiling_thickness: grosor del techo en tiles (ej. 1 o 2)
 * wall_thickness: grosor de paredes laterales en tiles (ej. 1 o 2)
 */
void level_build_border(DynamicLevel *level,
                        TileTextureDef floor_tile, uint8_t floor_thickness,
                        TileTextureDef ceiling_tile, uint8_t ceiling_thickness,
                        TileTextureDef wall_tile, uint8_t wall_thickness);

/**
 * Crea una plataforma horizontal en coordenadas de tile (tx, ty) de longitud 'length_tiles'.
 * is_oneway: true si es atravesable desde abajo (semi-solida), false si es bloque solido.
 */
void level_add_platform(DynamicLevel *level, uint16_t tx, uint16_t ty, uint16_t length_tiles,
                        TileTextureDef platform_tile, bool is_oneway);

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
 * Generador procedural/estilo plantilla: crea un nivel completo con limites, pisos,
 * paredes, techo y plataformas escalonadas utilizando el tema especificado.
 */
void level_generate_templated(DynamicLevel *level, uint16_t width_tiles, uint16_t height_tiles,
                              CD_TEXTURE *tileset_tim, const LevelThemeTextures *theme);

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
