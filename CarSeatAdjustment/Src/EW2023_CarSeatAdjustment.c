/**
 * @file EW2023_CarSeatAdjustment.c
 * @brief Image viewer demo
 *
 * @author Bridgetek
 *
 * @date 2019
 *
* Copyright (c) Bridgetek Pte Ltd.
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by Bridgetek.
* BRIDGETEK accept no responsibility for any issues resulting from its use.
* The developer of the final application incorporating any parts of this
* sample project is responsible for ensuring its safe and correct operation
* and for any consequences resulting from its use.
 */
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "Common.h"
#include "Platform.h"
#include "EVE_CoCmd.h"
#include "EW2023_CarSeatAdjustment.h"

typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t isPortTrait;
}t_EVE_Screen;

t_EVE_Screen g_ScreenInfo;

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext, *phost;
void EW2023_CarSeatAdjustment();
static void helper_RotateScreen();

int main(int argc, char* argv[])
{
	phost = s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);
	g_ScreenInfo.width = s_pHalContext->Width;
	g_ScreenInfo.height = s_pHalContext->Height;
	g_ScreenInfo.isPortTrait = 1;

#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
	Esd_Calibrate(s_pHalContext);
	Calibration_Save(s_pHalContext);
#endif

	Flash_Init(s_pHalContext, TEST_DIR "/Flash/BT81X_Flash_Seat_Adjustment.bin", "BT81X_Flash_Seat_Adjustment.bin");

	EVE_Util_clearScreen(s_pHalContext);

	char* info[] =
	{ "Seat adjustment demo for EW2023",
		"This sample demonstrate a seat adjustment",
		"Use BT817, LCD WVGA and FT900",
		""
	};

	while (TRUE) {
		WelcomeScreen(s_pHalContext, info);

		EW2023_CarSeatAdjustment();

		EVE_Util_clearScreen(s_pHalContext);

		EVE_Hal_close(s_pHalContext);
		EVE_Hal_release();

		/* Init HW Hal for next loop*/
		Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
		Calibration_Restore(s_pHalContext);
#endif
	}

	return 0;
}

/// ///////////////////////////////////////////////////////
int precision = 16, fraction = 4;

#define SCREEN_W g_ScreenInfo.width
#define SCREEN_H g_ScreenInfo.height
#define phost s_pHalContext
static inline int32_t PIXEL(double x)
{
	int32_t res =  x * precision;
	return res;
}

typedef struct tagPoint
{
	int x;
	int y;
} T_POINT;

typedef struct tagPresetInfo
{
	int left_right_index;
	int up_down_index;
	int rotate_in_out_index;
	int rotate_up_down_index;
} T_PRESET_INFO;

typedef struct {
	uint32_t index;
	uint32_t tag;
	uint16_t format;
	uint32_t addressFlash;
	uint32_t addressRamg;
	uint32_t w;
	uint32_t h;
}TILE;

TILE tile[] = {
	//id  has_tag?  format                        flash      ramg      w      h
	{0  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 0         ,0        ,0     ,0       },   // unified.blob                                                                   : 0       : 4096
};

enum TAG
{
	TAG_UP = 1,
	TAG_DOWN,
	TAG_LEFT,
	TAG_RIGHT,
	TAG_ROTATE_IN,
	TAG_ROTATE_OUT,
	TAG_ROTATE_UP,
	TAG_ROTATE_DOWN,
	TAG_MAX
};

enum ICON_INDEX
{
	ICN_UP = 0,
	ICN_DOWN,
	ICN_LEFT,
	ICN_RIGHT,
	ICN_ROTATE_IN,
	ICN_ROTATE_OUT,
	ICN_ROTATE_UP,
	ICN_ROTATE_DOWN,
	ICN_MAX
};

const int ADDR_ARRAY[] = { 
	4096   ,
	388096 ,
	400128 ,
	436928 ,
	820928 ,
	823232 ,
	825536 ,
	827840 ,
	830144 ,
	832448 ,
	834752 ,
	837056 ,
	839360 ,
	841664 ,
	843968 ,
	846272 ,
	848576 ,
	850880 ,
	853184 ,
	855488 ,
	857792 ,
	892096 ,
	897024 ,
	921216 ,
	955520 ,
	960448 ,
	984640 ,
	1018944,
	1023872,
	1048064,
	3028544,
	3029696,
	4964224 };

enum
{
	ADDR_IDX_BG_SEAT = 0,
	ADDR_IDX_MENU_ITEM_SELECT_BG = 1,
	ADDR_IDX_MENU = 2,
	ADDR_IDX_BOOTUP_BG,
	ADDR_IDX_ICON,
	ADDR_IDX_ICON_GREY = 12,
	ADDR_IDX_MOVING_BACK = 20,
	ADDR_IDX_MOVING_BASE,
	ADDR_IDX_MOVING_GROUND,
	ADDR_IDX_BACK,
	ADDR_IDX_BASE,
	ADDR_IDX_GROUND,
	ADDR_IDX_OUTLINE_BACK,
	ADDR_IDX_OUTLINE_BASE,
	ADDR_IDX_OUTLINE_GROUND,
	ADDR_IDX_BOOTUP_ANI_DATA,
	ADDR_IDX_BOOTUP_ANI_OBJECT,
	ADDR_IDX_BOOTUP_ANI_DATA_2,
	ADDR_IDX_BOOTUP_ANI_OBJECT_2,
};

const int MENU_ITEM_Y_ARRAY[] = { 60, 119, 179 };

#define ICN_NUM		8

// up/down, left/right, rotate in/rotate out, rotate up/rotate down
const T_POINT ICN_POS_ARR[] = { {584, 278}, {583, 356}, {378, 422}, {457, 421}, {349, 80}, {653, 80}, {229, 278}, {229, 357} };

int8_t ICON_STATE[] = { 1, 1, 1, 1, 1, 1, 1, 1 };

static int			menu_index = 0;
static int			pre_menu_index = -1;

#define MAX_ROTATE_IN_OUT				200
#define MAX_ROTATE_UP_DOWN				160
#define MAX_UP_DOWN						(40 / MOVE_DISTANCE)
#define MAX_LEFT_RIGHT					(40 / MOVE_DISTANCE)

#define MAX_ALPHA						255

#define MOVE_DISTANCE					0.25f		// moving the chair 1 pixel each time of adjustment
#define SPEED_ROTATE					3600.0f

enum
{
	PRESET_1 = 0,
	PRESET_2,
	MANUAL,
	DEFAULT,
};


void startApp();
void drawing();
void processEvents();

void clearScreen();
void startDisplay2();
void endDisplay2();


void drawChairBack(T_PRESET_INFO info, int image_address);
void drawChairGround(T_PRESET_INFO info, int image_address);
void drawChairBase(T_PRESET_INFO info, int image_address);

void transit(T_PRESET_INFO a, T_PRESET_INFO b);


static T_PRESET_INFO PresetInfoArr[4] = { {MAX_LEFT_RIGHT / 2, MAX_UP_DOWN / 4, MAX_ROTATE_IN_OUT / 4, MAX_ROTATE_UP_DOWN / 2},  { MAX_LEFT_RIGHT, MAX_UP_DOWN, MAX_ROTATE_IN_OUT, MAX_ROTATE_UP_DOWN }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

static int8_t g_moving_back = 0;
static int8_t g_moving_ground = 0;
static int8_t g_moving_whole = 0;

static int8_t TOUCH_ICON_ARR[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

static int8_t g_ani_break = 0;
static int8_t g_ani = 0;

#define SAVER_TIME					(5 * 60 * 1000)		// 5 mins
uint32_t saver_time_start = 0;
int8_t g_saver_flag = 0;

#define ICON_W						48
#define ICON_H						48

#define MENU_Y						59
#define MENU_W						200
#define MENU_H						181

#define MENU_ITEM_W					199
#define MENU_ITEM_H					60


// define chair
#define CHAIR_BACK_W				136
#define CHAIR_BACK_H				249
#define CHAIR_BACK_X				488
#define CHAIR_BACK_Y				102

#define CHAIR_GROUND_W				216
#define CHAIR_GROUND_H				110
#define CHAIR_GROUND_X				330
#define CHAIR_GROUND_Y				261

#define CHAIR_BASE_W				176
#define CHAIR_BASE_H				27
#define CHAIR_BASE_X				355
#define CHAIR_BASE_Y				372

void getTouchEvent(Gpu_Hal_Context_t *phost, int *x, int *y, int8_t *isTouch)
{
	*x = Gpu_Hal_Rd16(phost, REG_TOUCH_SCREEN_XY + 2);
	*y = Gpu_Hal_Rd16(phost, REG_TOUCH_SCREEN_XY + 4);
	*isTouch = !(Gpu_Hal_Rd16(phost, REG_TOUCH_RAW_XY) & 0x8000);
}



uint8_t readTag(Gpu_Hal_Context_t *phost) {
	uint8_t tag = Gpu_Hal_Rd32(phost, REG_TOUCH_TAG) & 0x7F;
#if 0
	char snum[5];

	// convert 123 to string [buf]
	itoa(tag, snum, 10);
	Gpu_CoCmd_Text(p, 0, 100, 31, 0, snum);
#endif

	return tag;

}

uchar8_t istouch(Gpu_Hal_Context_t *phost) {
	uchar8_t ret = !(Gpu_Hal_Rd16(phost, REG_TOUCH_RAW_XY) & 0x8000);
	return ret;
}


void drawBootupBackground()
{
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
	Gpu_CoCmd_SetBitmap(phost, ADDR_ARRAY[ADDR_IDX_BOOTUP_BG], COMPRESSED_RGBA_ASTC_4x4_KHR, SCREEN_W, SCREEN_H);
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX2F(0, 0));
	App_WrCoCmd_Buffer(phost, END());
}

void bootUp(bool_t checkSaver)
{
	return;
	/* Switch Flash to FULL Mode */
	FlashHelper_SwitchFullMode(phost);

	uint16_t frame = 0;

#define FRAME_COUNT_1    136
	for (uint16_t playtime = 0; playtime < 2; playtime++)
	{
		for (frame = 0; frame < FRAME_COUNT_1; frame++)
		{
			Gpu_CoCmd_Dlstart(phost);
			App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
			App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

			drawBootupBackground();

			Gpu_CoCmd_AnimFrame(phost, DispWidth / 2, DispHeight / 2, ADDR_ARRAY[ADDR_IDX_BOOTUP_ANI_OBJECT], frame);
			App_WrCoCmd_Buffer(phost, DISPLAY());
			Gpu_CoCmd_Swap(phost);
			App_Flush_Co_Buffer(phost);
			Gpu_Hal_WaitCmdfifo_empty(phost);

			if (checkSaver)
			{
				processEvents();
				if (!g_saver_flag)
				{
					return;
				}
			}
		}
	}

#define FRAME_COUNT_2    182
	for (frame = 0; frame < FRAME_COUNT_2; frame++)
	{
		Gpu_CoCmd_Dlstart(phost);
		App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
		App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

		drawBootupBackground();

		Gpu_CoCmd_AnimFrame(phost, DispWidth / 2, DispHeight / 2, ADDR_ARRAY[ADDR_IDX_BOOTUP_ANI_OBJECT_2], frame);
		App_WrCoCmd_Buffer(phost, DISPLAY());
		Gpu_CoCmd_Swap(phost);
		App_Flush_Co_Buffer(phost);
		Gpu_Hal_WaitCmdfifo_empty(phost);

		if (checkSaver)
		{
			processEvents();
			if (!g_saver_flag)
			{
				return;
			}
		}
	}
}

void drawSeatBackground()
{
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
	Gpu_CoCmd_SetBitmap(phost, ADDR_ARRAY[ADDR_IDX_BG_SEAT], COMPRESSED_RGBA_ASTC_4x4_KHR, SCREEN_W, SCREEN_H);
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX2F(0, 0));
	App_WrCoCmd_Buffer(phost, END());
}

void drawSelectMenuItem()
{
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
	Gpu_CoCmd_SetBitmap(phost, ADDR_ARRAY[ADDR_IDX_MENU_ITEM_SELECT_BG], COMPRESSED_RGBA_ASTC_4x4_KHR, MENU_ITEM_W, MENU_ITEM_H);
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX2F(0, PIXEL(MENU_ITEM_Y_ARRAY[menu_index])));
	App_WrCoCmd_Buffer(phost, END());

	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
	Gpu_CoCmd_SetBitmap(phost, ADDR_ARRAY[ADDR_IDX_MENU], COMPRESSED_RGBA_ASTC_4x4_KHR, MENU_W, MENU_H);
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX2F(0, PIXEL(MENU_Y)));
	App_WrCoCmd_Buffer(phost, END());
}

void clearScreen()
{
	Gpu_CoCmd_Dlstart(phost);
	Gpu_ClearScreen(phost);
	App_Flush_Co_Buffer(phost);
	Gpu_Hal_WaitCmdfifo_empty(phost);
	Gpu_Hal_Sleep(150);
}

void drawIcon(int alpha)
{
	ICON_STATE[ICN_UP] = PresetInfoArr[MANUAL].up_down_index != MAX_UP_DOWN;
	ICON_STATE[ICN_DOWN] = PresetInfoArr[MANUAL].up_down_index != 0;
	ICON_STATE[ICN_LEFT] = PresetInfoArr[MANUAL].left_right_index != MAX_LEFT_RIGHT;
	ICON_STATE[ICN_RIGHT] = PresetInfoArr[MANUAL].left_right_index != 0;
	ICON_STATE[ICN_ROTATE_IN] = PresetInfoArr[MANUAL].rotate_in_out_index != 0;
	ICON_STATE[ICN_ROTATE_OUT] = PresetInfoArr[MANUAL].rotate_in_out_index != MAX_ROTATE_IN_OUT;
	ICON_STATE[ICN_ROTATE_UP] = PresetInfoArr[MANUAL].rotate_up_down_index != MAX_ROTATE_UP_DOWN;
	ICON_STATE[ICN_ROTATE_DOWN] = PresetInfoArr[MANUAL].rotate_up_down_index != 0;

	int tag = TAG_UP;
	for (int i = 0; i < ICN_NUM; i++)
	{
		App_WrCoCmd_Buffer(phost, TAG(tag++));
		App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(i + 1));

		App_WrCoCmd_Buffer(phost, COLOR_A(alpha != MAX_ALPHA ? alpha : (TOUCH_ICON_ARR[i] && ICON_STATE[i] ? 100 : 255)));

		Gpu_CoCmd_SetBitmap(phost, ADDR_ARRAY[(ICON_STATE[i] ? ADDR_IDX_ICON : ADDR_IDX_ICON_GREY) + i], COMPRESSED_RGBA_ASTC_4x4_KHR, ICON_W, ICON_H);
		App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
		App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(ICN_POS_ARR[i].x), PIXEL(ICN_POS_ARR[i].y)));
		App_WrCoCmd_Buffer(phost, END());
	}
}

void drawChairBack(T_PRESET_INFO info, int image_address)
{
	float angle = info.rotate_in_out_index * 65535 / SPEED_ROTATE;
	int x = 59, y = 246;

	App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());

	Gpu_CoCmd_SetBitmap(phost, image_address, COMPRESSED_RGBA_ASTC_4x4_KHR, CHAIR_BACK_W, CHAIR_BACK_H);

	App_WrCoCmd_Buffer(phost, BITMAP_SIZE_H(CHAIR_BACK_W >> 9, CHAIR_BACK_H >> 9));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, CHAIR_BACK_W, CHAIR_BACK_H));

	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	Gpu_CoCmd_LoadIdentity(phost);

	Gpu_CoCmd_Translate(phost, x * 65536, y * 65536);
	Gpu_CoCmd_Rotate(phost, (int32_t)angle);
	Gpu_CoCmd_Translate(phost, -x * 65536, -y * 65536);
	Gpu_CoCmd_SetMatrix(phost);

	App_WrCoCmd_Buffer(phost, 
		VERTEX2F(
			PIXEL(CHAIR_BACK_X - info.left_right_index * MOVE_DISTANCE), 
			PIXEL(CHAIR_BACK_Y - info.up_down_index * MOVE_DISTANCE)));
	App_WrCoCmd_Buffer(phost, END());

	App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());
}

void drawChairGround(T_PRESET_INFO info, int image_address)
{
	float angle = info.rotate_up_down_index * 65535 / SPEED_ROTATE;
	int x = 197, y = 91;

	App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());

	Gpu_CoCmd_SetBitmap(phost, image_address, COMPRESSED_RGBA_ASTC_4x4_KHR, CHAIR_GROUND_W, CHAIR_GROUND_H);

	App_WrCoCmd_Buffer(phost, BITMAP_SIZE_H(CHAIR_GROUND_W >> 9, CHAIR_GROUND_H >> 9));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, CHAIR_GROUND_W, CHAIR_GROUND_H));
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));

	Gpu_CoCmd_LoadIdentity(phost);
	Gpu_CoCmd_Translate(phost, x * 65536, y * 65536);
	Gpu_CoCmd_Rotate(phost, (int32_t)angle);
	Gpu_CoCmd_Translate(phost, -x * 65536, -y * 65536);
	Gpu_CoCmd_SetMatrix(phost);

	App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(CHAIR_GROUND_X - info.left_right_index * MOVE_DISTANCE), PIXEL(CHAIR_GROUND_Y - info.up_down_index * MOVE_DISTANCE)));
	App_WrCoCmd_Buffer(phost, END());

	App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());
}

void drawChairBase(T_PRESET_INFO info, int image_address)
{
	//App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(3));
	Gpu_CoCmd_SetBitmap(phost, image_address, COMPRESSED_RGBA_ASTC_4x4_KHR, CHAIR_BASE_W, CHAIR_BASE_H);
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX2F(PIXEL(CHAIR_BASE_X - info.left_right_index * MOVE_DISTANCE), PIXEL(CHAIR_BASE_Y - info.up_down_index * MOVE_DISTANCE)));
	App_WrCoCmd_Buffer(phost, END());
}


void fadeInIcon()
{
	for (int16_t alpha = 0; alpha <= MAX_ALPHA; alpha += 5)
	{
		startDisplay2();

		drawSeatBackground();
		drawSelectMenuItem();

		drawChairGround(PresetInfoArr[DEFAULT], ADDR_ARRAY[ADDR_IDX_OUTLINE_GROUND]);
		drawChairBack(PresetInfoArr[DEFAULT], ADDR_ARRAY[ADDR_IDX_OUTLINE_BACK]);
		drawChairBase(PresetInfoArr[DEFAULT], ADDR_ARRAY[ADDR_IDX_OUTLINE_BASE]);

		drawChairGround(PresetInfoArr[MANUAL], ADDR_ARRAY[ADDR_IDX_GROUND]);
		drawChairBack(PresetInfoArr[MANUAL], ADDR_ARRAY[ADDR_IDX_BACK]);
		drawChairBase(PresetInfoArr[MANUAL], ADDR_ARRAY[ADDR_IDX_BASE]);

		drawIcon(alpha);

		endDisplay2();

		processEvents();
		if (g_ani_break)
		{
			break;
		}
	}
	App_WrCoCmd_Buffer(phost, COLOR_A(255));
}

void fadeInChair(T_PRESET_INFO b)
{
	int16_t alpha = 0;

	// fade in chair
	for (alpha = MAX_ALPHA; alpha >= 0; alpha -= 10)
	{
		startDisplay2();

		drawSeatBackground();
		drawSelectMenuItem();

		App_WrCoCmd_Buffer(phost, COLOR_A((int)alpha));

		drawChairBase(b, ADDR_ARRAY[ADDR_IDX_MOVING_BASE]);
		drawChairGround(b, ADDR_ARRAY[ADDR_IDX_MOVING_GROUND]);
		drawChairBack(b, ADDR_ARRAY[ADDR_IDX_MOVING_BACK]);

		App_WrCoCmd_Buffer(phost, COLOR_A((int)(MAX_ALPHA - alpha)));

		drawChairBase(b, ADDR_ARRAY[ADDR_IDX_BASE]);
		drawChairGround(b, ADDR_ARRAY[ADDR_IDX_GROUND]);
		drawChairBack(b, ADDR_ARRAY[ADDR_IDX_BACK]);

		endDisplay2();

		processEvents();
		if (g_ani_break)
		{
			break;
		}
	}
}

void fadeInOutline(T_PRESET_INFO ol, T_PRESET_INFO cur)
{
	int16_t alpha = 0;


	// animating outline of other preset

	for (alpha = 0; alpha <= MAX_ALPHA; alpha += 10)
	{
		startDisplay2();

		drawSeatBackground();
		drawSelectMenuItem();

		App_WrCoCmd_Buffer(phost, COLOR_A((int)(alpha)));
		drawChairBase(ol, ADDR_ARRAY[ADDR_IDX_OUTLINE_BASE]);
		drawChairGround(ol, ADDR_ARRAY[ADDR_IDX_OUTLINE_GROUND]);
		drawChairBack(ol, ADDR_ARRAY[ADDR_IDX_OUTLINE_BACK]);

		App_WrCoCmd_Buffer(phost, COLOR_A(255));
		drawChairBase(cur, ADDR_ARRAY[ADDR_IDX_BASE]);
		drawChairGround(cur, ADDR_ARRAY[ADDR_IDX_GROUND]);
		drawChairBack(cur, ADDR_ARRAY[ADDR_IDX_BACK]);

		endDisplay2();

		processEvents();
		if (g_ani_break)
		{
			break;
		}
	}
	App_WrCoCmd_Buffer(phost, COLOR_A(255));

	if (!g_ani_break && menu_index == MANUAL)
	{
		fadeInIcon();
	}
}

void transit(T_PRESET_INFO a, T_PRESET_INFO b)
{
#define GAP(x)				((x) > 0 ? 1 : -1)
#define CHECK(x,y,dis)		(dis > 0 ? x < y : x > y)

	T_PRESET_INFO t = a;
	const float speed_rate = 1;
	int32_t distance[] = { 0, 0, 0, 0 };
	bool_t		ani_flag = 0;
	enum { UP_DOWN, LEFT_RIGHT, ROTATE_IN_OUT, ROTATE_UP_DOWN };

	// up/down
	distance[UP_DOWN] = b.up_down_index - a.up_down_index;
	distance[LEFT_RIGHT] = b.left_right_index - a.left_right_index;
	distance[ROTATE_IN_OUT] = b.rotate_in_out_index - a.rotate_in_out_index;
	distance[ROTATE_UP_DOWN] = b.rotate_up_down_index - a.rotate_up_down_index;

	g_ani = 1;
	while (CHECK(t.up_down_index, b.up_down_index, distance[UP_DOWN]) ||
		CHECK(t.left_right_index, b.left_right_index, distance[LEFT_RIGHT]) ||
		CHECK(t.rotate_in_out_index, b.rotate_in_out_index, distance[ROTATE_IN_OUT]) ||
		CHECK(t.rotate_up_down_index, b.rotate_up_down_index, distance[ROTATE_UP_DOWN]))
	{
		ani_flag = 1;
		startDisplay2();

		drawSeatBackground();
		drawSelectMenuItem();
		drawChairBase(b, ADDR_ARRAY[ADDR_IDX_OUTLINE_BASE]);
		drawChairGround(b, ADDR_ARRAY[ADDR_IDX_OUTLINE_GROUND]);
		drawChairBack(b, ADDR_ARRAY[ADDR_IDX_OUTLINE_BACK]);


		if (CHECK(t.up_down_index, b.up_down_index, distance[UP_DOWN]))
			t.up_down_index += GAP(distance[UP_DOWN]);
		if (CHECK(t.left_right_index, b.left_right_index, distance[LEFT_RIGHT]))
			t.left_right_index += GAP(distance[LEFT_RIGHT]);
		if (CHECK(t.rotate_in_out_index, b.rotate_in_out_index, distance[ROTATE_IN_OUT]))
			t.rotate_in_out_index += GAP(distance[ROTATE_IN_OUT]);
		if (CHECK(t.rotate_up_down_index, b.rotate_up_down_index, distance[ROTATE_UP_DOWN]))
			t.rotate_up_down_index += GAP(distance[ROTATE_UP_DOWN]);

		drawChairBase(t, ADDR_ARRAY[ADDR_IDX_MOVING_BASE]);
		drawChairGround(t, ADDR_ARRAY[ADDR_IDX_MOVING_GROUND]);
		drawChairBack(t, ADDR_ARRAY[ADDR_IDX_MOVING_BACK]);
		endDisplay2();

		processEvents();
		if (g_ani_break)
		{
			break;
		}
	}

	if (!g_ani_break)
	{
		if (menu_index != MANUAL)
		{
			if (ani_flag) fadeInChair(b);
		}
		else
		{
			fadeInOutline(PresetInfoArr[DEFAULT], PresetInfoArr[MANUAL]);
		}
	}

	g_ani = 0;

	if (g_ani_break)
	{
		g_ani_break = 0;

		if (menu_index == MANUAL)
		{
			PresetInfoArr[MANUAL] = PresetInfoArr[pre_menu_index];
		}

		transit(t, PresetInfoArr[menu_index]);
	}
}

void drawing()
{
	drawSeatBackground();

	drawSelectMenuItem();

	switch (menu_index)
	{
	case PRESET_1:
		//drawChairGround(PresetInfoArr[PRESET_2], ADDR_ARRAY[ADDR_IDX_OUTLINE_GROUND]);
		//drawChairBack(PresetInfoArr[PRESET_2], ADDR_ARRAY[ADDR_IDX_OUTLINE_BACK]);
		//drawChairBase(PresetInfoArr[PRESET_2], ADDR_ARRAY[ADDR_IDX_OUTLINE_BASE]);

		drawChairGround(PresetInfoArr[PRESET_1], ADDR_ARRAY[ADDR_IDX_GROUND]);
		drawChairBack(PresetInfoArr[PRESET_1], ADDR_ARRAY[ADDR_IDX_BACK]);
		drawChairBase(PresetInfoArr[PRESET_1], ADDR_ARRAY[ADDR_IDX_BASE]);
		break;
	case PRESET_2:
		//drawChairGround(PresetInfoArr[PRESET_1], ADDR_ARRAY[ADDR_IDX_OUTLINE_GROUND]);
		//drawChairBack(PresetInfoArr[PRESET_1], ADDR_ARRAY[ADDR_IDX_OUTLINE_BACK]);
		//drawChairBase(PresetInfoArr[PRESET_1], ADDR_ARRAY[ADDR_IDX_OUTLINE_BASE]);

		drawChairGround(PresetInfoArr[PRESET_2], ADDR_ARRAY[ADDR_IDX_GROUND]);
		drawChairBack(PresetInfoArr[PRESET_2], ADDR_ARRAY[ADDR_IDX_BACK]);
		drawChairBase(PresetInfoArr[PRESET_2], ADDR_ARRAY[ADDR_IDX_BASE]);
		break;
	case MANUAL:
		drawChairGround(PresetInfoArr[DEFAULT], ADDR_ARRAY[ADDR_IDX_OUTLINE_GROUND]);
		drawChairBack(PresetInfoArr[DEFAULT], ADDR_ARRAY[ADDR_IDX_OUTLINE_BACK]);
		drawChairBase(PresetInfoArr[DEFAULT], ADDR_ARRAY[ADDR_IDX_OUTLINE_BASE]);

		drawChairGround(PresetInfoArr[MANUAL], ADDR_ARRAY[g_moving_whole || g_moving_ground ? ADDR_IDX_MOVING_GROUND : ADDR_IDX_GROUND]);
		drawChairBack(PresetInfoArr[MANUAL], ADDR_ARRAY[g_moving_whole || g_moving_back ? ADDR_IDX_MOVING_BACK : ADDR_IDX_BACK]);
		drawChairBase(PresetInfoArr[MANUAL], ADDR_ARRAY[g_moving_whole ? ADDR_IDX_MOVING_BASE : ADDR_IDX_BASE]);
		drawIcon(MAX_ALPHA);
		break;
	default:
		break;
	}
}

void startDisplay2()
{
	Gpu_CoCmd_Dlstart(phost);

	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));

	if (g_ScreenInfo.width > 2047) {
		precision = 8;
		fraction = 3;
		App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(fraction ));
	}else if (g_ScreenInfo.width > 4095) {
		precision = 4;
		fraction = 2;
		App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(fraction ));
	}else if (g_ScreenInfo.width > 8191) {
		precision = 2;
		fraction = 1;
		App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(fraction ));
	}else if (g_ScreenInfo.width > 16383) {
		precision = 1;
		fraction = 0;
		App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(fraction ));
	}
	App_WrCoCmd_Buffer(phost, TAG_MASK(1));
}

void endDisplay2() {

	App_WrCoCmd_Buffer(phost, TAG_MASK(0));

	/*send command display - to end display commands*/
	App_WrCoCmd_Buffer(phost, DISPLAY()); // display

	/*draw the new screen*/
	Gpu_CoCmd_Swap(phost);

	/*reset cmd index*/
	App_Flush_Co_Buffer(phost);

	/*wait until EVE is free*/
	Gpu_Hal_WaitCmdfifo_empty(phost);
}


void processEvents()
{
	// check BACK button pressed
	int x, y; int8_t touching;
	getTouchEvent(phost, &x, &y, &touching);

	if (touching)
	{
		saver_time_start = GET_CURR_MILLIS();

		g_saver_flag = 0;

		if (x < MENU_W && y > MENU_Y)
		{
			for (int i = 0; i < 3; i++)
			{
				if (y < MENU_ITEM_Y_ARRAY[i] + MENU_ITEM_H)
				{
					if (menu_index == i)
						return;

					pre_menu_index = menu_index;
					menu_index = i;

					switch (menu_index)
					{
					case PRESET_1:
						if (g_ani)
							g_ani_break = 1;
						else
						{
							g_ani_break = 0;
							transit(PresetInfoArr[pre_menu_index], PresetInfoArr[PRESET_1]);
						}
						break;
					case PRESET_2:
						if (g_ani)
							g_ani_break = 1;
						else
						{
							g_ani_break = 0;
							transit(PresetInfoArr[pre_menu_index], PresetInfoArr[PRESET_2]);
						}
						break;
					case MANUAL:
						if (g_ani)
							g_ani_break = 1;
						else
						{
							g_ani_break = 0;
							PresetInfoArr[MANUAL] = PresetInfoArr[pre_menu_index];
							transit(PresetInfoArr[pre_menu_index], PresetInfoArr[MANUAL]);
						}
						break;
					default:
						break;
					}

					return;
				}
			}
		}
	}
	else
	{
		// check timer
		if ((GET_CURR_MILLIS() - saver_time_start) >= SAVER_TIME)
		{
			g_saver_flag = 1;
		}
	}

	uint8_t tag = readTag(phost);

	//printf("tag = %d\n", tag);

	g_moving_back = touching && (tag == TAG_ROTATE_IN || tag == TAG_ROTATE_OUT);
	g_moving_ground = touching && (tag == TAG_ROTATE_UP || tag == TAG_ROTATE_DOWN);
	g_moving_whole = touching && (tag >= TAG_UP && tag <= TAG_RIGHT);

	memset(TOUCH_ICON_ARR, 0, sizeof(TOUCH_ICON_ARR) * sizeof(int8_t));
	if (tag >= TAG_UP && tag < TAG_MAX)
	{
		TOUCH_ICON_ARR[tag - TAG_UP] = 1;
	}

	switch (tag)
	{
	case TAG_UP:
		PresetInfoArr[MANUAL].up_down_index++;
		if (PresetInfoArr[MANUAL].up_down_index > MAX_UP_DOWN)
		{
			PresetInfoArr[MANUAL].up_down_index = MAX_UP_DOWN;
			g_moving_whole = 0;
		}
		break;
	case TAG_DOWN:
		PresetInfoArr[MANUAL].up_down_index--;
		if (PresetInfoArr[MANUAL].up_down_index < 0)
		{
			PresetInfoArr[MANUAL].up_down_index = 0;
			g_moving_whole = 0;
		}
		break;
	case TAG_LEFT:
		PresetInfoArr[MANUAL].left_right_index++;
		if (PresetInfoArr[MANUAL].left_right_index > MAX_LEFT_RIGHT)
		{
			PresetInfoArr[MANUAL].left_right_index = MAX_LEFT_RIGHT;
			g_moving_whole = 0;
		}
		break;
	case TAG_RIGHT:
		PresetInfoArr[MANUAL].left_right_index--;
		if (PresetInfoArr[MANUAL].left_right_index < 0)
		{
			PresetInfoArr[MANUAL].left_right_index = 0;
			g_moving_whole = 0;
		}
		break;
	case TAG_ROTATE_IN:
		PresetInfoArr[MANUAL].rotate_in_out_index--;
		if (PresetInfoArr[MANUAL].rotate_in_out_index < 0)
		{
			PresetInfoArr[MANUAL].rotate_in_out_index = 0;
			g_moving_back = 0;
		}
		break;
	case TAG_ROTATE_OUT:
		PresetInfoArr[MANUAL].rotate_in_out_index++;
		if (PresetInfoArr[MANUAL].rotate_in_out_index > MAX_ROTATE_IN_OUT)
		{
			PresetInfoArr[MANUAL].rotate_in_out_index = MAX_ROTATE_IN_OUT;
			g_moving_back = 0;
		}
		break;
	case TAG_ROTATE_UP:
		PresetInfoArr[MANUAL].rotate_up_down_index++;
		if (PresetInfoArr[MANUAL].rotate_up_down_index > MAX_ROTATE_UP_DOWN)
		{
			PresetInfoArr[MANUAL].rotate_up_down_index = MAX_ROTATE_UP_DOWN;
			g_moving_ground = 0;
		}
		break;
	case TAG_ROTATE_DOWN:
		PresetInfoArr[MANUAL].rotate_up_down_index--;
		if (PresetInfoArr[MANUAL].rotate_up_down_index < 0)
		{
			PresetInfoArr[MANUAL].rotate_up_down_index = 0;
			g_moving_ground = 0;
		}
		break;
	default:
		break;
	}
}

void appsetup()
{
	FlashHelper_SwitchFullMode(phost);
	Gpu_CoCmd_FlashFast(phost, 0);
	Gpu_CoCmd_FlashRead(phost, 4096, 4096, RAM_G_SIZE - 4096);
}

void clean()
{
	/* End of command sequence */
	Gpu_CoCmd_Dlstart(phost);
	Gpu_Copro_SendCmd(phost, CMD_STOP);
	App_Flush_Co_Buffer(phost);
	Gpu_Hal_WaitCmdfifo_empty(phost);
	Gpu_Hal_Sleep(1000);

	/* Close all the opened handles */
    Gpu_Hal_Close(phost);
    Gpu_Hal_DeInit();
}

static void mainLoop()
{
	saver_time_start = GET_CURR_MILLIS();

	while (TRUE)
	{
		startDisplay2();

		if (g_saver_flag)
		{
			bootUp(1);
		}
		else
		{
			drawing();
		}

		processEvents();
		endDisplay2();
	}
}

static void helper_RotateScreen() {
	if (g_ScreenInfo.isPortTrait) {
		g_ScreenInfo.width = s_pHalContext->Height;
		g_ScreenInfo.height = s_pHalContext->Width;
		g_ScreenInfo.isPortTrait = 0;
	}
	else {
		g_ScreenInfo.width = s_pHalContext->Width;
		g_ScreenInfo.height = s_pHalContext->Height;
		g_ScreenInfo.isPortTrait = 1;
	}
}

void EW2023_CarSeatAdjustment()
{
	appsetup();
	bootUp(0);
	mainLoop();
	clean();
}

/* Nothing beyond this */
