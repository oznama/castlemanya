#include <stdint.h>

#include <psxcd.h>
#include <psxgpu.h>

#include "cd_loader.h"

#define CD_SECTOR_SIZE 2048
#define CD_BUFFER_SIZE 163840
#define CD_MAX_FILE_SIZE (CD_BUFFER_SIZE - (CD_SECTOR_SIZE - 1))

/* Buffer estático compartido para evitar malloc durante la lectura desde CD. */
static uint32_t cd_raw_buffer[CD_BUFFER_SIZE / sizeof(uint32_t)];
static uint8_t cd_initialized;

static int cd_ensure_init(void)
{
	if (!cd_initialized)
	{
		if (!CdInit())
			return 0;
		cd_initialized = 1;
	}
	return 1;
}

static int cd_read_file_to_buffer(const char *path, int *out_file_size)
{
	CdlFILE file;
	int sector_count;
	int read_result;

	if (!cd_ensure_init())
		return -2;

	if (!CdSearchFile(&file, path))
		return -3;

	if (file.size <= 0 || file.size > CD_MAX_FILE_SIZE)
		return -4;

	sector_count = (file.size + CD_SECTOR_SIZE - 1) / CD_SECTOR_SIZE;
	if (!CdControl(CdlSetloc, &file.pos, 0))
		return -5;

	if (!CdRead(sector_count, cd_raw_buffer, CdlModeSpeed))
		return -6;

	read_result = CdReadSync(0, 0);
	if (read_result < 0)
		return -7;

	if (out_file_size)
		*out_file_size = file.size;

	return 0;
}

int cd_load_image(const char *path, CD_TEXTURE *texture)
{
	TIM_IMAGE tim;
	int read_status;

	read_status = cd_read_file_to_buffer(path, 0);
	if (read_status != 0)
		return read_status;

	/* GetTimInfo devuelve 0 cuando el TIM se interpreta correctamente. */
	if (GetTimInfo(cd_raw_buffer, &tim) != 0)
		return -8;

	LoadImage(tim.prect, tim.paddr);
	DrawSync(0);

	if (tim.mode & 0x8)
	{
		LoadImage(tim.crect, tim.caddr);
		DrawSync(0);
	}

	texture->mode = tim.mode;
	texture->image_x = tim.prect->x;
	texture->image_y = tim.prect->y;
	texture->clut_x = (tim.mode & 0x8) ? tim.crect->x : 0;
	texture->clut_y = (tim.mode & 0x8) ? tim.crect->y : 0;

	return 0;
}

char* cd_read_txt(const char* path)
{
	int file_size = 0;
	int read_status;
	char *text_buffer;

	read_status = cd_read_file_to_buffer(path, &file_size);
	if (read_status != 0)
		return 0;

	text_buffer = (char *)cd_raw_buffer;
	text_buffer[file_size] = '\0';

	return text_buffer;
}