#include "Transaction.h"
#include "Def.h"

#include "App.h"
#include "Event.h"

// Global variables
extern Gpu_Hal_Context_t * phost;
extern E_LANG g_language;

typedef enum {
    TR_PREPARE = 0,
    TR_ANI_SLIDER,
    TR_WAIT_START,
    TR_END
} TR_STATE;

static TR_STATE page_state = TR_PREPARE;

static int SLIDER_X = 365;
static int angle = 0;
static const int ROTATE_ANGLE = 1;

extern const char * s_transactionEstimation;
extern const char * s_energy;
extern const char * s_kWh;

extern const char * s_time;
extern const char * s_minutes;

extern const char * s_cost;
extern const char * s_currency;
extern const char * s_startCharging;

int16_t x_ani = 0;
static int16_t x_car = 0;
bool_t  start_ani = FALSE;
const int8_t X_ANI_STEP = 8;

static int16_t alpha;
static int16_t alpha_step;

static int16_t width_slider_ani = 0;

static float r_scale;
static float r_scale_step;
static const float ENERGY_SCALE = 0.03;

void tr_init() {
    angle = 0;
    start_ani = FALSE;
    x_ani = SCREEN_WIDTH - SLIDER_X + 50;
    x_car = 0;
    page_state = TR_PREPARE;

    alpha = 255;
    alpha_step = -5;
    width_slider_ani = 0;

    r_scale = 1.0;
    r_scale_step = ENERGY_SCALE;
}

void draw_energy() {

	r_scale += r_scale_step;
	if (r_scale >= 1.7) {
		r_scale = 1.7;
		r_scale_step = -ENERGY_SCALE;
	} else if (r_scale <= 1.0) {
		r_scale = 1.0;
		r_scale_step = ENERGY_SCALE;
	}

	uint32_t w = TR_CHARGE_TEXT.width * 2;
	uint32_t h = TR_CHARGE_TEXT.height * 2; //(uint32_t)(TR_CHARGE_TEXT.height * (1 + r_scale * 0.5));

	Gpu_CoCmd_SetBitmap(phost, ADDR(TR_CHARGE_TEXT, TR_BEGIN), COMPRESSED_RGBA_ASTC_4x4_KHR,
			TR_CHARGE_TEXT.width, TR_CHARGE_TEXT.height);
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, w, h));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE_H(w >> 9, h >> 9));

	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());

	Gpu_CoCmd_LoadIdentity(phost);
	Gpu_CoCmd_Scale(phost, r_scale * 65536, r_scale * 65536);
	Gpu_CoCmd_Translate(phost, 65536 , 65536);
	Gpu_CoCmd_SetMatrix(phost);


	uint32_t x = x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN - 10 - TR_CHARGE_TEXT.width * (r_scale - 1) * 0.5;
	uint32_t y = Y_CENTER(TR_CHARGE_TEXT, 435) - TR_CHARGE_TEXT.height * (r_scale - 1) * 0.5;


	App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x), PIXEL(y)));
	App_WrCoCmd_Buffer(phost, END());
	App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());
}

void tr_drawing() {

    Gpu_CoCmd_SetFont2(phost, HF_TITLE, FontTitle.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(phost, HF_SMALL, FontSmall.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(phost, HF_NUMBER, FontNumber.xf_addr - FontBegin.xf_addr, 0);

    // Draw Text: header
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
    Gpu_CoCmd_Text(phost, X_HEADER, Y_HEADER, HF_TITLE, 0, s_transactionEstimation);
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

    int16_t HF_START_CHARGING = 29;
    int16_t y_start_charging = Y_CENTER(TR_CHARGE_BUTTON, 490) + TR_CHARGE_BUTTON.height * 0.5 + 20;
    if (g_language == LANG_CN) {
        HF_START_CHARGING = HF_TITLE;
        y_start_charging += 15;
    }

    if (page_state == TR_PREPARE) {

        draw_image(phost, TR_CAR, ADDR(TR_CAR, TR_BEGIN), x_car, Y_CENTER(TR_CAR, 510));

        draw_image(phost, TR_CHARGE_BUTTON, ADDR(TR_CHARGE_BUTTON, TR_BEGIN),
                   x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width - TR_RIGHT_BUTTON_MARGIN,
                   Y_CENTER(TR_CHARGE_BUTTON, 490));

        draw_image(phost, TR_CHARGE_TEXT, ADDR(TR_CHARGE_TEXT, TR_BEGIN),
                   x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN - 10,
                   Y_CENTER(TR_CHARGE_TEXT, 435));

        App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
        Gpu_CoCmd_Text(phost,
                       x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN,
                       y_start_charging,
                       HF_START_CHARGING, OPT_CENTER, s_startCharging);
        App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

        x_car -= 5;
        x_ani -= X_ANI_STEP * 2;
        if (x_ani <= 0) {
            x_ani = 0;
            page_state = TR_ANI_SLIDER;
        }
        return;
    } else if (page_state == TR_ANI_SLIDER) {
        int32_t y_slider, x_slider;

        draw_image(phost, TR_CAR, ADDR(TR_CAR, TR_BEGIN), x_ani - TR_CAR.width * 0.5, Y_CENTER(TR_CAR, 510));

        draw_image(phost, TR_CHARGE_BUTTON, ADDR(TR_CHARGE_BUTTON, TR_BEGIN),
                   x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width - TR_RIGHT_BUTTON_MARGIN,
                   Y_CENTER(TR_CHARGE_BUTTON, 490));

        draw_image(phost, TR_CHARGE_TEXT, ADDR(TR_CHARGE_TEXT, TR_BEGIN),
                   x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN - 10,
                   Y_CENTER(TR_CHARGE_TEXT, 435));
        App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
        Gpu_CoCmd_Text(phost,
                       x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN,
                       y_start_charging,
                       HF_START_CHARGING, OPT_CENTER, s_startCharging);
        App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));


        // Draw Slider
        App_WrCoCmd_Buffer(phost, COLOR_RGB(219, 219, 219));
        App_WrCoCmd_Buffer(phost, LINE_WIDTH(SLIDER_HEIGHT));
        App_WrCoCmd_Buffer(phost, BEGIN(LINES));

        for (int i = 0; i < 3; i++) {
            y_slider = PIXEL(SLIDER_Y + i * SLIDER_GAP);
            x_slider = SLIDER_X + (SLIDER_WIDTH - width_slider_ani) * 0.5;
            App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_slider), y_slider));
            App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_slider + width_slider_ani), y_slider));
        }
        width_slider_ani += 20;

        if (width_slider_ani >= SLIDER_WIDTH) {
            page_state = TR_WAIT_START;
        }
        return;
    } else {
        // Draw Car
        draw_image(phost, TR_CAR, ADDR(TR_CAR, TR_BEGIN), x_ani - TR_CAR.width * 0.5, Y_CENTER(TR_CAR, 510));
        // Battery background
        draw_image(phost, TR_BATTERY, ADDR(TR_BATTERY, TR_BEGIN), SCREEN_WIDTH - TR_BATTERY.width - TR_RIGHT_MARGIN, Y_BATTERY);
        // Battery Red
        draw_image(phost, TR_BAT_RED, ADDR(TR_BAT_RED, TR_BEGIN), SCREEN_WIDTH - TR_BATTERY.width - TR_RIGHT_MARGIN + BATTERY_CELL_X[4], Y_BATTERY + BATTERY_CELL_Y);

        if (page_state == TR_END) {
            x_ani += X_ANI_STEP;
        } 
    }
  
    // Draw Charging Button Outter
    App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());
    App_WrCoCmd_Buffer(phost, TAG(TR_TAG_CHARGE_BUTTON));
    Gpu_CoCmd_SetBitmap(phost, ADDR(TR_CHARGE_BUTTON, TR_BEGIN), COMPRESSED_RGBA_ASTC_4x4_KHR,
                        TR_CHARGE_BUTTON.width, TR_CHARGE_BUTTON.height);
    App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, TR_CHARGE_BUTTON.width, TR_CHARGE_BUTTON.height));
    App_WrCoCmd_Buffer(phost, BITMAP_SIZE_H(TR_CHARGE_BUTTON.width >> 9, TR_CHARGE_BUTTON.height >> 9));
    
    App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
    Gpu_CoCmd_LoadIdentity(phost);
    Gpu_CoCmd_Translate(phost, 65536 * (TR_CHARGE_BUTTON.width - 1) / 2, 65536 * (TR_CHARGE_BUTTON.height - 1) / 2);
    Gpu_CoCmd_Rotate(phost, angle * 65536 / 360);
    Gpu_CoCmd_Translate(phost, -65536 * (TR_CHARGE_BUTTON.width - 1) / 2, -65536 * (TR_CHARGE_BUTTON.height - 1) / 2);
    Gpu_CoCmd_SetMatrix(phost);
    App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width - TR_RIGHT_BUTTON_MARGIN), 
                                       PIXEL(Y_CENTER(TR_CHARGE_BUTTON, 490))));
    App_WrCoCmd_Buffer(phost, END());

    angle += ROTATE_ANGLE;
    if (angle >= 360) {
        angle = 0;
    }
    App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());

    // Draw Charging Button Inner
    /*draw_image(phost, TR_CHARGE_TEXT, ADDR(TR_CHARGE_TEXT, TR_BEGIN),
                        x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN - 10,
                        Y_CENTER(TR_CHARGE_TEXT, 435));*/
    draw_energy();

    

    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(phost, TAG(TR_TAG_CHARGE_BUTTON));
    Gpu_CoCmd_Text(phost, 
                   x_ani + SCREEN_WIDTH - TR_CHARGE_BUTTON.width * 0.5 - TR_RIGHT_BUTTON_MARGIN,
                   y_start_charging,
                   HF_START_CHARGING, OPT_CENTER, s_startCharging);
    App_WrCoCmd_Buffer(phost, TAG(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

    // Draw Slider
    App_WrCoCmd_Buffer(phost, COLOR_RGB(219, 219, 219));
    App_WrCoCmd_Buffer(phost, LINE_WIDTH(SLIDER_HEIGHT));
    App_WrCoCmd_Buffer(phost, BEGIN(LINES));

    for (int i = 0; i < 3; i++) {
        App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_ani + SLIDER_X), PIXEL(SLIDER_Y + i*SLIDER_GAP)));
        App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_ani + SLIDER_X + SLIDER_WIDTH), PIXEL(SLIDER_Y + i * SLIDER_GAP)));
    }
    App_WrCoCmd_Buffer(phost, COLOR_RGB(104, 167, 22));
    App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_ani + SLIDER_X), PIXEL(SLIDER_Y)));
    App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(x_ani + SLIDER_X + SLIDER_WIDTH * 0.1), PIXEL(SLIDER_Y)));
    App_WrCoCmd_Buffer(phost, END());
    
    // Draw Number
    const int16_t HF_VALUE = HF_NUMBER;
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
    Gpu_CoCmd_SetFont2(phost, HF_TITLE, FontTitle.xf_addr - FontBegin.xf_addr, 0);
    // Energy
    Gpu_CoCmd_Number(phost, x_ani + SLIDER_X, SLIDER_Y - 55, HF_VALUE, 0, 25);
    Gpu_CoCmd_Number(phost, x_ani + SLIDER_X + SLIDER_WIDTH, SLIDER_Y - 55, 31, OPT_RIGHTX, 250);

    // Time
    Gpu_CoCmd_Number(phost, x_ani + SLIDER_X, SLIDER_Y + SLIDER_GAP - 55, HF_VALUE, 0, 0);
    Gpu_CoCmd_Number(phost, x_ani + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + SLIDER_GAP - 55, 31, OPT_RIGHTX, 120);

    // Cost
    Gpu_CoCmd_Number(phost, x_ani + SLIDER_X, SLIDER_Y + 2 * SLIDER_GAP - 55, HF_VALUE, 0, 0);
    Gpu_CoCmd_Number(phost, x_ani + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + 2 * SLIDER_GAP - 55, 31, OPT_RIGHTX, 32);

    // Draw Unit
    const int font_size = HF_SMALL;
    int unit_gap = 12;

    if (g_language == LANG_CN) {
    	unit_gap = 15;
    }

    App_WrCoCmd_Buffer(phost, COLOR_GREY);
    Gpu_CoCmd_Text(phost, x_ani + SLIDER_X, SLIDER_Y + unit_gap, font_size, 0, s_energy);
    Gpu_CoCmd_Text(phost, x_ani + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + unit_gap, font_size, OPT_RIGHTX, s_kWh);

    Gpu_CoCmd_Text(phost, x_ani + SLIDER_X, SLIDER_Y + SLIDER_GAP + unit_gap, font_size, 0, s_time);
    Gpu_CoCmd_Text(phost, x_ani + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + SLIDER_GAP + unit_gap, font_size, OPT_RIGHTX, s_minutes);

    Gpu_CoCmd_Text(phost, x_ani + SLIDER_X, SLIDER_Y + 2 * SLIDER_GAP + unit_gap, font_size, 0, s_cost);
    Gpu_CoCmd_Text(phost, x_ani + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + 2 * SLIDER_GAP + unit_gap, font_size, OPT_RIGHTX, s_currency);
    
    
}

void tr_process_event() {

    uint8_t tag = Gesture_Get(phost)->tagReleased;
    if (tag == TR_TAG_CHARGE_BUTTON && page_state == TR_WAIT_START) {
        page_state = TR_END;
        restart_screen_saver();
    }

    if (page_state == TR_END && SLIDER_X + x_ani >= 660) {
        switch_next_page();
    }
}
