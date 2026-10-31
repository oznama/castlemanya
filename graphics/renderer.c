#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <psxgpu.h>

#include "renderer.h"

#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define OT_LENGTH 16
#define BUFFER_LENGTH 8192

typedef struct {
	DISPENV disp_env;
	DRAWENV draw_env;
	uint32_t ot[OT_LENGTH];
	uint8_t buffer[BUFFER_LENGTH];
} RenderBuffer;

typedef struct {
	RenderBuffer buffers[2];
	uint8_t *next_packet;
	int active_buffer;
} RenderContext;

static RenderContext render_context;

static void setup_context(RenderContext *ctx, int w, int h, int r, int g, int b) {
	SetDefDrawEnv(&(ctx->buffers[0].draw_env), 0, 0, w, h);
	SetDefDispEnv(&(ctx->buffers[0].disp_env), 0, 0, w, h);
	SetDefDrawEnv(&(ctx->buffers[1].draw_env), 0, h, w, h);
	SetDefDispEnv(&(ctx->buffers[1].disp_env), 0, h, w, h);

	setRGB0(&(ctx->buffers[0].draw_env), r, g, b);
	setRGB0(&(ctx->buffers[1].draw_env), r, g, b);
	ctx->buffers[0].draw_env.isbg = 1;
	ctx->buffers[1].draw_env.isbg = 1;

	ctx->active_buffer = 0;
	ctx->next_packet = ctx->buffers[0].buffer;
	ClearOTagR(ctx->buffers[0].ot, OT_LENGTH);
	SetDispMask(1);
}

static void *new_primitive(RenderContext *ctx, int z, size_t size) {
	RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);
	uint8_t *primitive = ctx->next_packet;

	addPrim(&(buffer->ot[z]), primitive);
	ctx->next_packet += size;
	assert(ctx->next_packet <= &(buffer->buffer[BUFFER_LENGTH]));

	return primitive;
}

void renderer_init(void) {
	ResetGraph(0);
	FntLoad(960, 0);
	setup_context(&render_context, SCREEN_XRES, SCREEN_YRES, 0, 0, 0);
}

void renderer_present(void) {
	DrawSync(0);
	VSync(0);

	RenderBuffer *draw_buffer = &(render_context.buffers[render_context.active_buffer]);
	RenderBuffer *display_buffer = &(render_context.buffers[render_context.active_buffer ^ 1]);

	PutDispEnv(&(display_buffer->disp_env));
	DrawOTagEnv(
		&(draw_buffer->ot[OT_LENGTH - 1]),
		&(draw_buffer->draw_env)
	);

	render_context.active_buffer ^= 1;
	render_context.next_packet = display_buffer->buffer;
	ClearOTagR(display_buffer->ot, OT_LENGTH);
}

void renderer_draw_rect(int x, int y, int width, int height, int r, int g, int b) {
	TILE *tile = (TILE *) new_primitive(&render_context, 1, sizeof(TILE));

	setTile(tile);
	setXY0(tile, x, y);
	setWH(tile, width, height);
	setRGB0(tile, r, g, b);
}

void renderer_draw_text(int x, int y, int z, const char *text) {
	RenderBuffer *buffer = &(render_context.buffers[render_context.active_buffer]);

	render_context.next_packet = (uint8_t *)
		FntSort(&(buffer->ot[z]), render_context.next_packet, x, y, text);

	assert(
		render_context.next_packet <=
		&(buffer->buffer[BUFFER_LENGTH])
	);
}
