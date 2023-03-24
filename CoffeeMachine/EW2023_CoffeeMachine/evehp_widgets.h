#ifndef _EVEHP_WIDGETS_H_
#define _EVEHP_WIDGETS_H_
#include "platform.h"
#include "CoPro_Cmds.h"
#include "App_Common.h"

void evehp_widgets_msgbox(Gpu_Hal_Context_t *phost, char *msg, int x, int y, ft_uint8_t font, ft_uint8_t opt);
#define evehp_widgets_msgbox_default(phost, msg) evehp_widgets_msgbox(phost, msg, DispWidth / 2, DispHeight / 2, 30, OPT_CENTERX)

void evehp_widget_box(Gpu_Hal_Context_t *phost, int x, int y, int w, int h, uint32_t rounder);

#endif // _EVEHP_WIDGETS_H_
