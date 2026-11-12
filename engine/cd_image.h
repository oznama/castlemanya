#pragma once
#include <psxgpu.h>

typedef struct {
    uint32_t mode;
    uint16_t image_x;
    uint16_t image_y;
    uint16_t clut_x;
    uint16_t clut_y;
} CD_TEXTURE;

int cd_load_image(const char *path, CD_TEXTURE *texture);
