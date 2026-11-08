#include <sys/types.h>
#include <assert.h>

#include "graphic.h"

#define OT_LEN			256
#define PACKET_LEN		1024

#define SCREEN_XRES		320
#define SCREEN_YRES		240

typedef struct {
	DISPENV	disp;
	DRAWENV	draw;
	u_long 	ot[OT_LEN];
	char 	p[PACKET_LEN];
} DB;

DB db[2];
uint8_t db_active;
char *db_nextpri;

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

void render_rect(int x, int y, int width, int height, int r, int g, int b) {
	TILE *tile = (TILE *) new_primitive(1, sizeof(TILE));
	setTile(tile);
	setXY0(tile, x, y);
	setWH(tile, width, height);
	setRGB0(tile, r, g, b);
}

void render_text(uint8_t x, uint8_t y, uint8_t z, const char *text) {
	DB *buffer = &(db[db_active]);
	db_nextpri = (uint8_t *) FntSort(&(buffer->ot[z]), db_nextpri, x, y, text);
	assert(db_nextpri <= &(buffer->p[PACKET_LEN]));
}

void render_sprt(TIM_IMAGE tim, uint8_t x, uint8_t y, uint32_t width, uint32_t height, uint8_t u, uint8_t v, uint8_t r, uint8_t g, uint8_t b) {
	const unsigned int max_size_allowed = 256;

	// Validaciones para garantizar que el sprite no sea mayor de 256x256
	assert(width > 0 && width <= max_size_allowed);
	assert(height > 0 && height <= max_size_allowed);
	assert(u + width <= max_size_allowed);
	assert(v + height <= max_size_allowed);

	SPRT *sprt = (SPRT *)new_primitive(0, sizeof(SPRT));
	setSprt(sprt);
	setXY0(sprt, x, y); // Coordenadas en pantalla
	setRGB0(sprt, r, g, b); // Colores RGB
	setUV0(sprt, u, v); // Posicion en el sprite
	setWH(sprt, width, height); // Ancho y alto tomado del sprite (porcion de la imagen)

	if ( tim.mode & 0x8 ) {
		setClut(sprt, tim.crect->x, tim.crect->y);
	}

	DR_TPAGE *tpri = (DR_TPAGE*)new_primitive(0, sizeof(DR_TPAGE));
	setDrawTPage(tpri, 0, 0, getTPage(tim.mode & 3, 0, tim.prect->x, tim.prect->y));
}