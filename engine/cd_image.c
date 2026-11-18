#include <stdint.h>

#include <psxcd.h>
#include <psxgpu.h>

#include "cd_image.h"

#define CD_IMAGE_SECTOR_SIZE 2048
#define CD_IMAGE_BUFFER_SIZE 163840
#define CD_IMAGE_MAX_FILE_SIZE (CD_IMAGE_BUFFER_SIZE - (CD_IMAGE_SECTOR_SIZE - 1))

/* Buffer estático para evitar malloc durante la carga; limita TIM a ~160 KiB. */
static uint32_t cd_image_buffer[CD_IMAGE_BUFFER_SIZE / sizeof(uint32_t)];
static uint8_t cd_image_initialized;

int cd_load_image(const char *path, CD_TEXTURE *texture)
{
	CdlFILE file;
	int sector_count;
	int read_result;

	if (!cd_image_initialized)
	{
		if (!CdInit())
			return -2;
		cd_image_initialized = 1;
	}

	if (!CdSearchFile(&file, path))
		return -3;

	if (file.size <= 0 || file.size > CD_IMAGE_MAX_FILE_SIZE)
		return -4;

	sector_count = (file.size + CD_IMAGE_SECTOR_SIZE - 1) / CD_IMAGE_SECTOR_SIZE;
	if (!CdControl(CdlSetloc, &file.pos, 0))
		return -5;

	if (!CdRead(sector_count, cd_image_buffer, CdlModeSpeed))
		return -6;

	read_result = CdReadSync(0, 0);
	if (read_result < 0)
		return -7;

	TIM_IMAGE tim;

	/* GetTimInfo devuelve 0 cuando el TIM se interpreta correctamente. */
	if (GetTimInfo(cd_image_buffer, &tim) != 0)
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
