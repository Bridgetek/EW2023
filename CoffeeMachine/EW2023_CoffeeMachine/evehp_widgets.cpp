#include "Platform.h"
#include "evehp_widgets.h"

void evehp_widgets_msgbox(Gpu_Hal_Context_t *phost, char *msg, int x, int y, ft_uint8_t font, ft_uint8_t opt)
{
    Gpu_CoCmd_Dlstart(phost);
    App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));

    Gpu_CoCmd_Text(phost, x, y, font, opt, msg);

    /* Download the DL into DL RAM */
    App_WrCoCmd_Buffer(phost, DISPLAY_EVE());
    App_Flush_Co_Buffer(phost);
    App_Flush_DL_Buffer(phost);
    Gpu_Hal_DLSwap(phost, DLSWAP_FRAME);
    Gpu_Hal_WaitCmdfifo_empty(phost);
}

void evehp_widget_box(Gpu_Hal_Context_t *phost, int x, int y, int w, int h, uint32_t rounder){
	App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
	App_WrCoCmd_Buffer(phost, LINE_WIDTH(rounder * 16));
	App_WrCoCmd_Buffer(phost, VERTEX2F( (x + rounder)* 16, (y + rounder) * 16));
	App_WrCoCmd_Buffer(phost, VERTEX2F((x + w-rounder) * 16, (y + h-rounder) * 16));
}