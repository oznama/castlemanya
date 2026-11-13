#include <sys/types.h>
#include <assert.h>

#include "graphic.h"

#define OT_LEN			256
#define PACKET_LEN		1024

typedef struct {
	DISPENV	disp;
	DRAWENV	draw;
	u_long 	ot[OT_LEN];
	char 	p[PACKET_LEN];
} DB;

DB db[2];
uint8_t db_active;
char *db_nextpri;

void valida_sprite_size(uint32_t w, uint32_t h, uint8_t u, uint8_t v);

void graphic_init() {

	ResetGraph( 0 );
	FntLoad(960, 0);

	SetDefDispEnv( &db[0].disp, 0, 0, SCREEN_XRES, SCREEN_YRES );
	SetDefDrawEnv( &db[0].draw, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES );

	// db[0].disp.isinter = 1; // Esto veo que lo aplica el imbecil, pero no le veo cambio

	setRGB0( &db[0].draw, 0, 0, 0 );
	db[0].draw.isbg = 1;
	db[0].draw.dtd = 1;

	SetDefDispEnv( &db[1].disp, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES );
	SetDefDrawEnv( &db[1].draw, 0, 0, SCREEN_XRES, SCREEN_YRES );
	setRGB0( &db[1].draw, 0, 0, 0 );
	db[1].draw.isbg = 1;
	db[1].draw.dtd = 1;

	PutDrawEnv( &db[0].draw );

	ClearOTagR( db[0].ot, OT_LEN );
	ClearOTagR( db[1].ot, OT_LEN );

	db_nextpri = db[0].p;

}

void graphic_display() {
	DrawSync( 0 );
	VSync( 0 );

	db_active ^= 1;
	db_nextpri = db[db_active].p;

	ClearOTagR( db[db_active].ot, OT_LEN );

	PutDrawEnv( &db[db_active].draw );
	PutDispEnv( &db[db_active].disp );

	SetDispMask( 1 );

	DrawOTag( db[1-db_active].ot+(OT_LEN-1) );
}

static void *new_primitive(int z, size_t size) {
	DB *buffer = &(db[db_active]);
	char *primitive = db_nextpri;
	addPrim(&(buffer->ot[z]), primitive);
	db_nextpri += size;
	assert(db_nextpri <= &(buffer->p[PACKET_LEN]));
	return (void *) primitive;
}

void render_rect(int x, int y, int w, int h, int r, int g, int b) {
	TILE *tile = (TILE *) new_primitive(1, sizeof(TILE));
	setTile(tile);
	setXY0(tile, x, y);
	setWH(tile, w, h);
	setRGB0(tile, r, g, b);
}

void render_text(uint8_t x, uint8_t y, uint8_t z, const char *text) {
	DB *buffer = &(db[db_active]);
	db_nextpri = (uint8_t *) FntSort(&(buffer->ot[z]), db_nextpri, x, y, text);
	assert(db_nextpri <= &(buffer->p[PACKET_LEN]));
}

void render_sprt(CD_TEXTURE tim, uint8_t x, uint8_t y, uint32_t w, uint32_t h, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b) {
	// valida_sprite_size(w, h, u, v);

	SPRT *sprt = (SPRT *)new_primitive(0, sizeof(SPRT));
	setSprt(sprt);
	setXY0(sprt, x, y); // Coordenadas en pantalla
	setRGB0(sprt, r, g, b); // Colores RGB
	setUV0(sprt, u, v); // Posicion en el sprite
	setWH(sprt, w, h); // Ancho y alto tomado del sprite (porcion de la imagen)

	if ( tim.mode & 0x8 ) {
		setClut(sprt, tim.clut_x, tim.clut_y);
	}

	DR_TPAGE *tpri = (DR_TPAGE*)new_primitive(0, sizeof(DR_TPAGE));
	setDrawTPage(tpri, 0, 0, getTPage(tim.mode & 3, 0, tim.image_x, tim.image_y));
}

void render_poly_ft4(CD_TEXTURE tim, uint8_t x, uint8_t y, uint32_t w, uint32_t h, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b, uint8_t rotateu, uint8_t rotatev) {
	// valida_sprite_size(w, h, u, v);

	POLY_FT4 *quad = (POLY_FT4 *)new_primitive(0, sizeof(POLY_FT4));
    setPolyFT4(quad);
    setXYWH(quad, x, y, w, h);
    setRGB0(quad, r, g, b);

	if ( tim.mode & 0x8 ) {
		setClut(quad, tim.clut_x, tim.clut_y);
	}

    quad->tpage = getTPage(tim.mode & 3, 0, tim.image_x, tim.image_y);

	uint16_t u_end = (uint16_t)((uint16_t)u + w - 1u);
	uint16_t v_end = (uint16_t)((uint16_t)v + h - 1u);
	setUV4(quad,
		rotateu ? (uint8_t)u_end : u, rotatev ? (uint8_t)v_end : v,
		rotateu ? u : (uint8_t)u_end, rotatev ? (uint8_t)v_end : v,
		rotateu ? (uint8_t)u_end : u, rotatev ? v : (uint8_t)v_end,
		rotateu ? u : (uint8_t)u_end, rotatev ? v : (uint8_t)v_end
	);
}

void valida_sprite_size(uint32_t w, uint32_t h, uint8_t u, uint8_t v) {
	const unsigned int max_size_allowed = 256;

	// Validaciones para garantizar que el sprite no sea mayor de 256x256
	assert(w > 0 && w <= max_size_allowed);
	assert(h > 0 && h <= max_size_allowed);
	assert(u + w <= max_size_allowed);
	assert(v + h <= max_size_allowed);
}