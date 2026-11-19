#include <stdint.h>
#include <psxgpu.h>
#include "loader.h"
#include "../graphics/graphic.h"

extern const uint32_t loader_tim[];

void loader_show(void)
{

    TIM_IMAGE tim;

    GetTimInfo(loader_tim, &tim);

    LoadImage(tim.prect, tim.paddr);
    DrawSync(0);

    if (tim.mode & 0x8)
    {
        LoadImage(tim.crect, tim.caddr);
        DrawSync(0);
    }

    CD_TEXTURE loader_texture;
    loader_texture.mode = tim.mode;
    loader_texture.image_x = tim.prect->x;
    loader_texture.image_y = tim.prect->y;
    loader_texture.clut_x = (tim.mode & 0x8) ? tim.crect->x : 0;
    loader_texture.clut_y = (tim.mode & 0x8) ? tim.crect->y : 0;

    render_sprt(loader_texture, 32, 112, 128, 128, 0, 0, 128, 128, 128);

    render_text(200, 220, 0, "JON LEIDER...");

    graphic_display();
    DrawSync(0);
    VSync(0);
}