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

static void *new_primitive(int z, size_t size) {
	DB *buffer = &(db[db_active]);
	char *primitive = db_nextpri;

	addPrim(&(buffer->ot[z]), primitive);
	db_nextpri += size;
	assert(db_nextpri <= &(buffer->p[PACKET_LEN]));

	return primitive;
}

void graphic_init() {

    ResetGraph( 0 );
    FntLoad(960, 0);

	SetDefDispEnv( &db[0].disp, 0, 0, SCREEN_XRES, SCREEN_YRES );
	SetDefDrawEnv( &db[0].draw, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES );
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

void render_tim(TIM_IMAGE tim, uint8_t x, uint8_t y) {
    int depth = tim.mode & 3;
    int pixels_per_word = (depth == 0) ? 4 : (depth == 1) ? 2 : 1;
    int width = tim.prect->w * pixels_per_word;
    int height = tim.prect->h;

    POLY_FT4 *quad = (POLY_FT4*)db_nextpri;
    setPolyFT4(quad);
    setXYWH(quad, x, y, width, height);
    setRGB0(quad, 128, 128, 128);

    quad->tpage = getTPage(tim.mode & 3, 0, tim.prect->x, tim.prect->y);
    setUV4(quad, 0, 0, 255, 0, 0, 63, 255, 63);
    addPrim(db[db_active].ot + (OT_LEN - 1), quad);

    db_nextpri = (char *) (quad + 1);
}