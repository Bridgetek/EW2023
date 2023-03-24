#include <SPI.h>
#include <SD.h>
#include "Platform.h"
#include "App_Common.h"

#include "vm801p.h"
#include "Platform.h" 
#include "App_Common.h"
#include "image.h"
#include "printf.h"
#include "Gesture.h"
#include "evehp_widgets.h"

#define EVE_millis millis

eve_im_t eveim[] = {
{118  ,86 , 4096, 14272},    // americano_118x86_PALETTED4444_index.raw              : 4096   : 10176 
{480  ,272, 14784, 145344},  // bg_480x272_PALETTED4444_index.raw                    : 14784  : 130560
{231  ,24 , 145664, 151232}, // Brewing in Progress..._231x24_PALETTED4444_index.raw : 145664 : 5568  
{97   ,106, 151744, 162048}, // brewing_97x106_PALETTED4444_index.raw                : 151744 : 10304 
{60   ,30 , 162560, 164416}, // btn selected big_60x30_PALETTED4444_index.raw        : 162560 : 1856  
{50   ,30 , 164864, 166400}, // btn selected_50x30_PALETTED4444_index.raw            : 164864 : 1536  
{50   ,20 , 166912, 167936}, // btn unselected big_50x20_PALETTED4444_index.raw      : 166912 : 1024  
{40   ,20 , 168256, 169088}, // btn unselected_40x20_PALETTED4444_index.raw          : 168256 : 832   
{122  ,40 , 169344, 174272}, // btn-brew_122x40_PALETTED4444_index.raw               : 169344 : 4928  
{38   ,46 , 174784, 176576}, // btn-left_38x46_PALETTED4444_index.raw                : 174784 : 1792  
{38   ,46 , 177088, 178880}, // btn-right_38x46_PALETTED4444_index.raw               : 177088 : 1792  
{122  ,40 , 179392, 184320}, // btn-start_122x40_PALETTED4444_index.raw              : 179392 : 4928  
{118  ,86 , 184832, 195008}, // cappuchino_118x86_PALETTED4444_index.raw             : 184832 : 10176 
{58   ,9  , 195520, 196096}, // Coffee Size_58x9_PALETTED4444_index.raw              : 195520 : 576   
{83   ,12 , 196224, 197248}, // Coffee Strength_83x12_PALETTED4444_index.raw         : 196224 : 1024  
{170  ,15 , 197376, 199936}, // Dark Coffee Roast_170x15_PALETTED4444_index.raw      : 197376 : 2560  
{191  ,23 , 200448, 204864}, // Enjoy your coffee!_191x23_PALETTED4444_index.raw     : 200448 : 4416  
{93   ,68 , 205376, 211712}, // enjoy__coffee_93x68_PALETTED4444_index.raw           : 205376 : 6336  
{175  ,25 , 212224, 216640}, // Good Morning!_175x25_PALETTED4444_index.raw          : 212224 : 4416  
{6    ,7  , 217152, 217216}, // L_6x7_PALETTED4444_index.raw                         : 217152 : 64    
{174  ,21 , 217280, 220992}, // Light Coffee Roast_174x21_PALETTED4444_index.raw     : 217280 : 3712  
{8    ,7  , 221504, 221568}, // M_8x7_PALETTED4444_index.raw                         : 221504 : 64    
{201  ,15 , 221696, 224768}, // Medium Coffee Roast_201x15_PALETTED4444_index.raw    : 221696 : 3072  
{52   ,12 , 225280, 225920}, // Milk Type_52x12_PALETTED4444_index.raw               : 225280 : 640   
{23   ,9  , 226048, 226304}, // Milk_23x9_PALETTED4444_index.raw                     : 226048 : 256   
{118  ,86 , 226368, 236544}, // mocha_118x86_PALETTED4444_index.raw                  : 226368 : 10176 
{15   ,7  , 237056, 237184}, // Oat_15x7_PALETTED4444_index.raw                      : 237056 : 128   
{5    ,7  , 237376, 237440}, // S_5x7_PALETTED4444_index.raw                         : 237376 : 64    
{40   ,7  , 237504, 237824}, // Skimmed_40x7_PALETTED4444_index.raw                  : 237504 : 320   
{16   ,10 , 238208, 238400}, // Soy_16x10_PALETTED4444_index.raw                     : 238208 : 192   
{50   ,38 , 238592, 240512}, // steam_50x38_PALETTED4444_index.raw                   : 238592 : 1920  
{168  ,8  , 241024, 242368}, // strength 1_168x8_PALETTED4444_index.raw              : 241024 : 1344  
{168  ,8  , 242880, 244224}, // strength 2_168x8_PALETTED4444_index.raw              : 242880 : 1344  
{168  ,8  , 244736, 246080}, // strength 3_168x8_PALETTED4444_index.raw              : 244736 : 1344  
{168  ,8  , 246592, 247936}, // strength 4_168x8_PALETTED4444_index.raw              : 246592 : 1344  
{84   ,9  , 248448, 249216}, // Sweetness Level_84x9_PALETTED4444_index.raw          : 248448 : 768   
{11   ,9  , 249344, 249472}, // warning_11x9_PALETTED4444_index.raw                  : 249344 : 128   
{9    ,12 , 249536, 249664}, // water_9x12_PALETTED4444_index.raw                    : 249536 : 128   
{26   ,7  , 249856, 250048}, // Whole_26x7_PALETTED4444_index.raw                    : 249856 : 192   
}; 

#define eveim_americano                0
#define eveim_bg                       1
#define eveim_Brewing_in_Progress___   2
#define eveim_brewing                  3
#define eveim_btn_selected_big         4
#define eveim_btn_selected             5
#define eveim_btn_unselected_big       6
#define eveim_btn_unselected           7
#define eveim_btn_brew                 8
#define eveim_btn_left                 9
#define eveim_btn_right                10
#define eveim_btn_start                11
#define eveim_cappuchino               12
#define eveim_Coffee_Size              13
#define eveim_Coffee_Strength          14
#define eveim_Dark_Coffee_Roast        15
#define eveim_Enjoy_your_coffee_       16
#define eveim_enjoy__coffee            17
#define eveim_Good_Morning_            18
#define eveim_L                        19
#define eveim_Light_Coffee_Roast       20
#define eveim_M                        21
#define eveim_Medium_Coffee_Roast      22
#define eveim_Milk_Type                23
#define eveim_Milk                     24
#define eveim_mocha                    25
#define eveim_Oat                      26
#define eveim_S                        27
#define eveim_Skimmed                  28
#define eveim_Soy                      29
#define eveim_steam                    30
#define eveim_strength_1               31
#define eveim_strength_2               32
#define eveim_strength_3               33
#define eveim_strength_4               34
#define eveim_Sweetness_Level          35
#define eveim_warning                  36
#define eveim_water                    37
#define eveim_Whole                    38

#define msg(x) Serial.println(F(x))
#define EVE_Cmd_wr32 App_WrCoCmd_Buffer

/* Global used for buffer optimization */
Gpu_Hal_Context_t host, *phost;
static void SAMAPP_GPU_Ball_Stencil();
void drawing(eve_im_t *eveim);
void process_event(eve_im_t *eveim);

enum tags
{
  tag_none = 1,
  tag_left,
  tag_right,
  tag_start,
  tag_brew,
  tag_cf_size1,
  tag_cf_size2,
  tag_cf_size3,
  tag_cf_strength,
  tag_cf_mtype1,
  tag_cf_mtype2,
  tag_cf_mtype3,
  tag_cf_mtype4,
  tag_cf_milk,
  tag_cf_sweet,
  tag_,
};

const uint16_t appx = 0, appy = 0, appw = 480, apph = 272;

#define PAGE_1 0
#define PAGE_2 1
#define PAGE_3 2
#define PAGE_4 3
#define PAGE_TEST PAGE_2 // for debugging purose

#ifdef PAGE_TEST
static uint8_t page_nth = PAGE_TEST;
#else
static uint8_t page_nth = PAGE_1;
#endif

#define color_btn_selected       App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 255, 255));
#define p2_color_text_selected   App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
#define p2_color_text_unselected App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
#define color_white              App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

static uint8_t p1_cf_num = 3;
static uint8_t p1_cf_type = 1;
static const uint8_t p2_coffee_num = 3;
static uint8_t p2_select_coffee = 1;
static uint8_t p2_select_size = 1;
static uint8_t p2_select_strength = eveim_strength_1;
static uint8_t p2_select_milk_type = 1;
static uint8_t p2_select_milk = eveim_strength_1;
static uint8_t p2_select_sweet = eveim_strength_1;

void ino_print(char *s)
{
  Serial.print(s);
}

void setup_SD()
{
  while (1)
    if (SD.begin(SD_CSPIN))
    {
      break;
    }
  msg("SD card mounted");
}

/* setup */
void setup()
{
  Serial.begin(115200);
  delay(1000);

  msg("Init SD card...");
  setup_SD();

  /* Init HW Hal */
  msg("Init HAL...");
  phost = &host;
  App_Common_Init(&host);

/* Screen Calibration*/
#ifndef PAGE_TEST
  App_Calibrate_Screen(&host);
#endif

/* Init ramg */
// evehp_widgets_msgbox_default(phost, "Copying files to ramg...");
#define tlen 360
  evehp_widgets_msgbox(phost, "Loading images into EVE's RAM_G", ((appx + appw) - tlen) / 2, (appy + apph - 20) / 2, 29, 0);
  int s = copy_file_2_ramg(phost, "e23cfm.bin", 0);
  if (!s)
  {
    while (1)
      evehp_widgets_msgbox_default(phost, "Error open flash");
  }
}

void loop()
{
  msg("Drawing");
  while (1)
  {
    drawing(eveim);
    process_event(eveim);
  }
}

static void draw_btn(int im_id, int x, int y, int scale, int tag){
  EVE_Cmd_wr32(phost, TAG(tag));
  if (Gesture_Get()->tagPressed == tag){
    color_btn_selected;
    draw_eve_im(phost, &eveim[im_id], x, y, scale);
    color_white;
  }else{
    draw_eve_im(phost, &eveim[im_id], x, y, scale);
  }
}

static void tagBox(int x, int y, int w, int h, int tag)
{
  EVE_Cmd_wr32(phost, COLOR_A(0));
  EVE_Cmd_wr32(phost, TAG(tag));
  evehp_widget_box(phost, x, y, w, h, 6);
  EVE_Cmd_wr32(phost, COLOR_A(255));
}

static void trackBox(int x, int y, int w, int h, int tag)
{
  #define expand 8
  EVE_Cmd_wr32(phost, COLOR_A(0));
  Gpu_CoCmd_Track(phost, x, y-expand, w, h+expand*2, tag);
  EVE_Cmd_wr32(phost, TAG(tag));
  evehp_widget_box(phost, x, y-expand, w, h+expand*2, 6);
  EVE_Cmd_wr32(phost, COLOR_A(255));
}

static int getLine(int32_t trackval)
{
  int32_t pc = trackval * 100 / 65536;
  // APP_INF("pc=%u", pc);
  if (pc <= 25)
  {
    return eveim_strength_1;
  }
  else if (pc <= 50)
  {
    return eveim_strength_2;
  }
  else if (pc <= 75)
  {
    return eveim_strength_3;
  }
  else if (pc <= 100)
  {
    return eveim_strength_4;
  }
}

static int p2_text_color(int select, int unselect, int condition, int value){
  int eveim_box = unselect;
  if (value == condition)
  {
    p2_color_text_selected
  }else{
    p2_color_text_unselected;
  }

}
static int getBoxIm(int select, int unselect, int condition, int value)
{
  int eveim_box = unselect;
  if (value == condition)
  {
    eveim_box = select;
  }else{
  }

  return eveim_box;
}

void drawing_page1(eve_im_t *eveim)
{
  int padding = 15; // padding from roast text and button left/right
  int btn_w = eveim[eveim_btn_left].w;
  int roast_max_w = 0;
  int y = 131;
  int x = appw/2 - eveim[eveim_Good_Morning_].w / 2;

  // good morning text
  draw_eve_im(phost, &eveim[eveim_Good_Morning_], x, 57, 0);

  // Coffee roast text
  int roast[] = {eveim_Light_Coffee_Roast, eveim_Medium_Coffee_Roast, eveim_Dark_Coffee_Roast};
  for (char i=0; i < 3; i++){
    int img_id = roast[i];
    int img_w = eveim[img_id].w;

    if  (p1_cf_type == i)
    {
      x = appw/2 - img_w / 2;
      draw_eve_im(phost, &eveim[img_id], x, y, 0);
    }
  
    // also find the max roast text length 
    roast_max_w = roast_max_w > img_w? roast_max_w : img_w;
  }

  // Button left
  x = appw/2 - (btn_w *2 + padding * 2 + roast_max_w) / 2;
  draw_btn(eveim_btn_left, x, 113, 0, tag_left);

  // Button right
  x = appw/2 + padding + roast_max_w / 2;
  draw_btn(eveim_btn_right, x, 113, 0, tag_right);

  // Button start
  roast_max_w = eveim[eveim_btn_start].w;
  x = appw/2 - roast_max_w / 2;
  draw_btn(eveim_btn_start, x, 205, 0, tag_start);
}

void drawing_page2(eve_im_t *eveim)
{
  // center
  EVE_Cmd_wr32(phost, COLOR_A(35));
  EVE_Cmd_wr32(phost, COLOR_RGB(0xF6,0xFC,0xFC));
  evehp_widget_box(phost, 36, 29, 168, 168, 6);
  EVE_Cmd_wr32(phost, COLOR_A(255));
  EVE_Cmd_wr32(phost, COLOR_RGB(255, 255, 255));

  draw_btn(eveim_btn_left, 2, 90, 0, tag_left);
  draw_btn(eveim_btn_right, 203, 90, 0, tag_right);
  draw_btn(eveim_btn_brew, 64, 211, 0, tag_brew);

  EVE_Cmd_wr32(phost, TAG(tag_right));
  if (p2_select_coffee == 1)
    draw_eve_im(phost, &eveim[eveim_cappuchino], 60, 74, 0);
  else if (p2_select_coffee == 2)
    draw_eve_im(phost, &eveim[eveim_americano], 60, 74, 0);
  else
    draw_eve_im(phost, &eveim[eveim_mocha], 60, 74, 0);

  EVE_Cmd_wr32(phost, TAG(tag_none));
  draw_eve_im(phost, &eveim[eveim_Coffee_Size], 270, 29, 0);

  int eveim_box, x=0, y=0, eveim_text = 0;
  const int box_xy[][2] = {
    {3, 3}, // selected big
    {3, 3}, // selected
    {0, 0}, // unselected big
    {0, 0}, // unselected
  };
  int box_xy_start = eveim_btn_selected_big;


  // coffee size S
  eveim_box = getBoxIm(eveim_btn_selected, eveim_btn_unselected, 1, p2_select_size);
  tagBox(270, 42, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_size1);
  draw_btn(eveim_box, 270 - box_xy[eveim_box-box_xy_start][0], 42 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_size1);
  p2_text_color(eveim_btn_selected, eveim_btn_unselected, 1, p2_select_size);
  // Gpu_CoCmd_Text(phost, 270 + 15, 42, 27, 0, "S");
  eveim_text = eveim_S;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 270 + x, 42 + y, 0);
  color_white;

  // coffee size M
  eveim_box = getBoxIm(eveim_btn_selected, eveim_btn_unselected, 2, p2_select_size);
  tagBox(325, 42, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_size2);
  draw_btn(eveim_box, 325 - box_xy[eveim_box-box_xy_start][0], 42 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_size2);
  p2_text_color(eveim_btn_selected, eveim_btn_unselected, 2, p2_select_size);
  // Gpu_CoCmd_Text(phost, 325 + 15, 42, 27, 0, "M");
  eveim_text = eveim_M;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 325 + x, 42 + y, 0);
  color_white;

  // coffee size L
  eveim_box = getBoxIm(eveim_btn_selected, eveim_btn_unselected, 3, p2_select_size);
  tagBox(380, 42, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_size3);
  draw_btn(eveim_box, 380 - box_xy[eveim_box-box_xy_start][0], 42 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_size3);
  p2_text_color(eveim_btn_selected, eveim_btn_unselected, 3, p2_select_size);
  // Gpu_CoCmd_Text(phost, 380 + 15, 42, 27, 0, "L");
  eveim_text = eveim_L;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 380 + x, 42 + y, 0);
  color_white;

  // coffee Strength
  draw_eve_im(phost, &eveim[eveim_Coffee_Strength], 270, 77, 0);
  draw_btn(p2_select_strength, 270, 90, 0, tag_cf_strength);
  trackBox(270, 90, eveim[eveim_strength_1].w, eveim[eveim_strength_1].h, tag_cf_strength);
  draw_eve_im(phost, &eveim[eveim_Milk_Type], 270, 113, 0);

  // Milk type Whole
  eveim_box = getBoxIm(eveim_btn_selected, eveim_btn_unselected, 1, p2_select_milk_type);
  tagBox(270, 126, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_mtype1);
  draw_btn(eveim_box, 270 - box_xy[eveim_box-box_xy_start][0], 126 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_mtype1);
  p2_text_color(eveim_btn_selected, eveim_btn_unselected, 1, p2_select_milk_type);
  // Gpu_CoCmd_Text(phost, 270 + 4, 128, 20, 0, "whole");
  eveim_text = eveim_Whole;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 270 + x, 126 + y, 0);
  color_white;

  // Milk type skimmed
  eveim_box = getBoxIm(eveim_btn_selected_big, eveim_btn_unselected_big, 2, p2_select_milk_type);
  tagBox(325, 126, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_mtype2);
  draw_btn(eveim_box, 325 - box_xy[eveim_box-box_xy_start][0], 126 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_mtype2);
  p2_text_color(eveim_btn_selected_big, eveim_btn_unselected_big, 2, p2_select_milk_type);
  // Gpu_CoCmd_Text(phost, 325 + 3, 128, 20, 0, "Skimmed");
  eveim_text = eveim_Skimmed;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 325 + x + 3, 126 + y, 0);
  color_white;

  // Milk type soy
  eveim_box = getBoxIm(eveim_btn_selected, eveim_btn_unselected, 3, p2_select_milk_type);
  tagBox(390, 126, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_mtype3);
  draw_btn(eveim_box, 390 - box_xy[eveim_box-box_xy_start][0], 126 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_mtype3);
  p2_text_color(eveim_btn_selected, eveim_btn_unselected, 3, p2_select_milk_type);
  // Gpu_CoCmd_Text(phost, 390 + 11, 128, 20, 0, "Soy");
  eveim_text = eveim_Soy;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 390 + x, 126 + y, 0);
  color_white;

  // Milk type oat
  eveim_box = getBoxIm(eveim_btn_selected, eveim_btn_unselected, 4, p2_select_milk_type);
  tagBox(270, 154, eveim[eveim_box].w, eveim[eveim_box].h, tag_cf_mtype4);
  draw_btn(eveim_box, 270 - box_xy[eveim_box-box_xy_start][0], 154 - box_xy[eveim_box-box_xy_start][1], 0, tag_cf_mtype4);
  p2_text_color(eveim_btn_selected, eveim_btn_unselected, 4, p2_select_milk_type);
  // Gpu_CoCmd_Text(phost, 270 + 11, 156, 20, 0, "Oat");
  eveim_text = eveim_Oat;
  x = eveim[eveim_box].w/2 - eveim[eveim_text].w/2 - (eveim[eveim_box].w - 40)/2;
  y = eveim[eveim_box].h/2 - eveim[eveim_text].h/2 - (eveim[eveim_box].h - 20)/2;
  draw_eve_im(phost, &eveim[eveim_text], 270 + x, 154 + y, 0);
  color_white;

  // Milk
  draw_eve_im(phost, &eveim[eveim_Milk], 270, 185, 0);
  draw_btn(p2_select_milk, 270, 198, 0,tag_cf_milk);
  trackBox(270, 198, eveim[eveim_strength_1].w, eveim[eveim_strength_1].h, tag_cf_milk);

  // Sweetless level
  draw_eve_im(phost, &eveim[eveim_Sweetness_Level], 270, 221, 0);
  draw_btn(p2_select_sweet, 270, 234, 0,tag_cf_sweet);
  trackBox(270, 234, eveim[eveim_strength_1].w, eveim[eveim_strength_1].h, tag_cf_sweet);
  color_white;
}

void drawing_page3(eve_im_t *eveim)
{
#define presicion 500
  int32_t yoffset = 110, ymax = 45 * presicion, g = 10;
  static int32_t y[] = {10 * presicion, 22 * presicion, 32 * presicion};
  for (char i = 0; i < 3; i++)
  {
    y[i] = y[i] + y[i] * g / 200;
    if (y[i] >= ymax)
    {
      y[i] = presicion;
    }
  }
  static uint32_t timeStartMs = 0;
  uint32_t now = millis();
  const uint32_t dur = 30000;

  if (timeStartMs == 0)
  {
    timeStartMs = now;
  }

  if (now - timeStartMs >= dur)
  {
    // reset and switch page
    timeStartMs = 0;
#ifndef PAGE_TEST
    page_nth = PAGE_4;
#endif
  }

  draw_eve_im(phost, &eveim[eveim_Brewing_in_Progress___], 122, 51, 0);
  draw_eve_im(phost, &eveim[eveim_brewing], 190, 108, 0);
  draw_eve_im(phost, &eveim[eveim_water], 227, yoffset + y[0] / presicion, 0);
  draw_eve_im(phost, &eveim[eveim_water], 246, yoffset + y[1] / presicion, 0);
  draw_eve_im(phost, &eveim[eveim_water], 235, yoffset + y[2] / presicion, 0);
}

void drawing_page4(eve_im_t *eveim)
{
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;

  const int32_t translate_speed = 1;
  static int32_t translate_y = 0;

  static uint32_t timeStartMs = 0;
  uint32_t now = EVE_millis();
  const uint32_t dur = 5000;

  // start timer
  if (timeStartMs == 0)
  {
    timeStartMs = now;
  }

  // when timed out
  if (now - timeStartMs >= dur)
  {
    // reset and switch page
    timeStartMs = 0;
    y = 0;
#ifndef PAGE_TEST
    page_nth = PAGE_1;
#endif
  }

  // set translate_y
  translate_y += translate_speed;
  if (translate_y > eveim[eveim_steam].h)
  {
    translate_y = -(int32_t)(eveim[eveim_steam].h);
    // translate_y = 0;
  }

  w = eveim[eveim_steam].w;
  h = eveim[eveim_steam].h;
  x = appw / 2 - w/2;// - translate_y;
  y = 110;
  // EVE_Cmd_wr32(phost, SCISSOR_XY(x, y));
  // EVE_Cmd_wr32(phost, SCISSOR_SIZE(w, h));
  draw_eve_im_translate(phost, &eveim[eveim_steam], x, y, 0, translate_y);
  // EVE_Cmd_wr32(phost, SCISSOR_XY(0, 0));
  // EVE_Cmd_wr32(phost, SCISSOR_SIZE(480, 272));

  w = eveim[eveim_Enjoy_your_coffee_].w;
  x = appw / 2 - w/2;
  y = 51;
  draw_eve_im(phost, &eveim[eveim_Enjoy_your_coffee_], x, y, 0);

  w = eveim[eveim_enjoy__coffee].w;
  x = appw / 2 - w/2;
  y = 150;
  draw_eve_im(phost, &eveim[eveim_enjoy__coffee], x, y, 0);
}

void drawing(eve_im_t *eveim)
{
  Gpu_CoCmd_Dlstart(phost);
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

  draw_eve_im(phost, &eveim[eveim_bg], 0, 0, 0);
  EVE_Cmd_wr32(phost, SCISSOR_XY(0, 0));
  EVE_Cmd_wr32(phost, SCISSOR_SIZE(480, 272));

  if (page_nth == PAGE_1)
    drawing_page1(eveim);
  else if (page_nth == PAGE_2)
    drawing_page2(eveim);
  else if (page_nth == PAGE_3)
    drawing_page3(eveim);
  else if (page_nth == PAGE_4)
    drawing_page4(eveim);

  /* Download the DL into DL RAM */
  App_WrCoCmd_Buffer(phost, DISPLAY_EVE());
  App_Flush_Co_Buffer(phost);
  App_Flush_DL_Buffer(phost);
  Gpu_Hal_DLSwap(phost, DLSWAP_FRAME);
  Gpu_Hal_WaitCmdfifo_empty(phost);
}
void process_event(eve_im_t *eveim)
{
  Gesture_Touch_t *gt = Gesture_Renew(phost);

  // if (gt->tagReleased)
  //   APP_INF("tag: %u", gt->tagReleased);

  if (gt->tagReleased == tag_start || gt->tagReleased == tag_brew)
  {
    page_nth++;
  }
  else if (gt->tagReleased == tag_left)
  {
    if (page_nth == PAGE_1)
    {
      if (p1_cf_type == 0)
      {
        p1_cf_type = p1_cf_num - 1;
      }
      else
      {
        p1_cf_type--;
      }
    }
    else if (page_nth == PAGE_2)
    {
      p2_select_coffee--;
      if (p2_select_coffee == 0)
      {
        p2_select_coffee = p2_coffee_num;
      }
    }
  }
  else if (gt->tagReleased == tag_right)
  {
    if (page_nth == PAGE_1)
    {
      if (p1_cf_type == p1_cf_num - 1)
      {
        p1_cf_type = 0;
      }
      else
      {
        p1_cf_type++;
      }
    }
    else if (page_nth == PAGE_2)
    {
      p2_select_coffee++;
      if (p2_select_coffee > p2_coffee_num)
      {
        p2_select_coffee = 0;
      }
    }
  }
  else if (gt->tagReleased == tag_cf_size1)
  {
    p2_select_size = 1;
  }
  else if (gt->tagReleased == tag_cf_size2)
  {
    p2_select_size = 2;
  }
  else if (gt->tagReleased == tag_cf_size3)
  {
    p2_select_size = 3;
  }
  else if (gt->tagReleased == tag_cf_mtype1)
  {
    p2_select_milk_type = 1;
  }
  else if (gt->tagReleased == tag_cf_mtype2)
  {
    p2_select_milk_type = 2;
  }
  else if (gt->tagReleased == tag_cf_mtype3)
  {
    p2_select_milk_type = 3;
  }
  else if (gt->tagReleased == tag_cf_mtype4)
  {
    p2_select_milk_type = 4;
  }
  else if (gt->tagReleased &&  page_nth == PAGE_3)
  {
    page_nth = PAGE_4;
  }
  else if (gt->tagReleased &&  page_nth == PAGE_4)
  {
    page_nth = PAGE_1;
  }

  // track object
  if (gt->tagTrackTouched == tag_cf_strength)
  {
    p2_select_strength = getLine(gt->trackValLine);
  }
  else if (gt->tagTrackTouched == tag_cf_milk)
  {
    p2_select_milk = getLine(gt->trackValLine);
  }
  else if (gt->tagTrackTouched == tag_cf_sweet)
  {
    p2_select_sweet = getLine(gt->trackValLine);
  }
}