#include <SPI.h>
#include <SD.h>

#include "evehp_widgets.h"
#include "image.h"
#include "printf.h"

#define CHUNK_SIZE UART_BUFF_SIZE
extern char __buff[UART_BUFF_SIZE];
int copy_file_2_ramg(Gpu_Hal_Context_t *phost, char *file, int ramg) {
  uint32_t fsize = 10;
  uint32_t num = 0;
  uint32_t percent = 0;
  File myFile = SD.open(file, FILE_READ);
  fsize = myFile.size();

  if (myFile) {
    uint32_t remain = fsize;
    uint32_t written = 0;

    while (remain > 0) {

      percent = 100 - remain * 100 / fsize;
      // snprintf(__buff, UART_BUFF_SIZE, "Initializing RAMG %u%%", percent);evehp_widgets_msgbox_default(phost, __buff);
      APP_INF("Initializing RAMG %u%%", percent);

      num = CHUNK_SIZE;
      if (CHUNK_SIZE > remain) {
        num = remain;
      }

      myFile.read(__buff, num);
      Gpu_Hal_WrMem(phost, ramg + written, __buff, num);
      written += num;
      remain -= num;

      if (written >= RAM_G_SIZE) {
        Serial.println(F("Error: File is > ramg size"));
        break;
      }
    }

    myFile.close();
  } else {
    // if the file didn't open, print an error:
    // Serial.println("error opening file");
    APP_INF("error opening file %s", file);
  }

  return fsize;
}

void draw_eve_im(Gpu_Hal_Context_t *phost, eve_im_t *img, int x, int y, int scale_10)
{
  return draw_eve_im_translate(phost, img, x, y, scale_10, 0);
}

void draw_eve_im_translate(Gpu_Hal_Context_t *phost, eve_im_t *img, int x, int y, int scale_10, int32_t translatey)
{
    uint32_t iw = img->w;
    uint32_t ih = img->h;
    char is_save_context = 0;

    if (scale_10 or translatey)
    {
      is_save_context = 1;
    }

    if (scale_10)
    {
        iw = iw * scale_10 / 10;
        ih = ih * scale_10 / 10;
        // APP_DBG_U(iw);
        // APP_DBG_U(ih);
    }

    if (is_save_context)
    {
        App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());
    }

    App_WrCoCmd_Buffer(phost, PALETTE_SOURCE(img->ramg ));
    Gpu_CoCmd_SetBitmap(phost, img->ramg_index, PALETTED4444, iw, ih);
    
    App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
    
    if (scale_10)
    {
        APP_DBG_U(translatey);
        Gpu_CoCmd_LoadIdentity(phost);
        Gpu_CoCmd_Scale(phost, scale_10 * 65536 / 10, scale_10 * 65536 / 10 * scale_10 / 10);
        Gpu_CoCmd_SetMatrix(phost);
    }
    
    if (translatey)
    {
      Gpu_CoCmd_LoadIdentity(phost);
      Gpu_CoCmd_Translate(phost, 0 , translatey * 65536);
      Gpu_CoCmd_SetMatrix(phost);
      y -= translatey;
    }

    App_WrCoCmd_Buffer(phost, VERTEX2F(x * 16, y * 16));

    if (is_save_context)
    {
        App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());
    }
}
