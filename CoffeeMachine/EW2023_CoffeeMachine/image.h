#ifndef _IMAGE_H_
#define _IMAGE_H_
#include "platform.h"
#include "CoPro_Cmds.h"
#include "App_Common.h"

typedef struct eve_im_t_{
    ft_uint16_t w, h;
    ft_uint32_t ramg_index, ramg; // paletted image use index and lut
}eve_im_t;

int copy_file_2_ramg(Gpu_Hal_Context_t *phost, char *file, int ramg);
void draw_eve_im(Gpu_Hal_Context_t *phost, eve_im_t *img, int x, int y, int scale);
void draw_eve_im_translate(Gpu_Hal_Context_t *phost, eve_im_t *img, int x, int y, int scale, int32_t translatey);

#endif
