/**
 * @file EW2023_CarDashBoard.c
 * @brief Car DashBoard demo
 *
 * Please connect button to the MCU souser can siwth UI1 and UI2 with the button
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
#if defined(_WIN32)
#include <conio.h>
#endif

#include "Common.h"
#include "Platform.h"
#include "EVE_CoCmd.h"
#include "EW2023_CarDashBoard.h"
#include "Eve_Ext_Callist.h"

EVE_HalContext s_halContext;
EVE_HalContext* s_pHalContext, * phost;
#define MSG(M, ...)             printf("" M "\r\n", ##__VA_ARGS__)

void Input_To_Flash();
void EW2023_CarDashBoard();

int main(int argc, char* argv[])
{
	phost = s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);
	if (1) {
		//		Use ExtSync module by REG_PCLK==1 and set REG_PCLK_FREQ to:
		//		78 Mhz (0x8D1) for 1280x800 resolution
		//		54 MHz (0x891) for 1440x540 resolution
		//		12 MHz (0x21) for 1280x120 resolution.
#if defined(DISPLAY_RESOLUTION_WXGA)
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x8D1);
#elif defined(DISPLAY_RESOLUTION_USP)
#if EVE_SUPPORT_GEN == EVE4
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x891);
#endif
#elif defined(DISPLAY_RESOLUTION_UWB)
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x21);
#endif

		MSG("REG_HCYCLE        : %d   ", EVE_Hal_rd16(phost, REG_HCYCLE));
		MSG("REG_HOFFSET       : %d   ", EVE_Hal_rd32(phost, REG_HOFFSET));
		MSG("REG_HSYNC0        : %d   ", EVE_Hal_rd32(phost, REG_HSYNC0));
		MSG("REG_HSYNC1        : %d   ", EVE_Hal_rd32(phost, REG_HSYNC1));
		MSG("REG_VCYCLE        : %d   ", EVE_Hal_rd32(phost, REG_VCYCLE));
		MSG("REG_VOFFSET       : %d   ", EVE_Hal_rd32(phost, REG_VOFFSET));
		MSG("REG_VSYNC0        : %d   ", EVE_Hal_rd32(phost, REG_VSYNC0));
		MSG("REG_VSYNC1        : %d   ", EVE_Hal_rd32(phost, REG_VSYNC1));
		MSG("REG_SWIZZLE       : %d   ", EVE_Hal_rd32(phost, REG_SWIZZLE));
		MSG("REG_PCLK_POL      : %d   ", EVE_Hal_rd32(phost, REG_PCLK_POL));
		MSG("REG_HSIZE         : %d   ", EVE_Hal_rd32(phost, REG_HSIZE));
		MSG("REG_VSIZE         : %d   ", EVE_Hal_rd32(phost, REG_VSIZE));
		MSG("REG_CSPREAD       : %d   ", EVE_Hal_rd32(phost, REG_CSPREAD));
		MSG("REG_DITHER        : %d   ", EVE_Hal_rd32(phost, REG_DITHER));
		MSG("REG_OUTBITS       : %d   ", EVE_Hal_rd32(phost, REG_OUTBITS));
		MSG("REG_PCLK          : %d   ", EVE_Hal_rd32(phost, REG_PCLK));
#if EVE_SUPPORT_GEN == EVE4
		MSG("REG_PCLK_FREQ     : 0x%3x", EVE_Hal_rd32(phost, REG_PCLK_FREQ));
#endif
		MSG("REG_FREQUENCY     : %d   ", EVE_Hal_rd32(phost, REG_FREQUENCY));
		MSG("System clock: %d (Hz)    ", Get_SystemClock(phost));
	}

	EVE_Util_loadSdCard(s_pHalContext);

	Input_To_Flash();

	EVE_Util_clearScreen(s_pHalContext);

	char* info[] =
	{ "Car Dashboard demo",
	 "This sample demonstrate a car dashboard",
	 "Use BT817, LCD 1440x540 and FT900",
	 "Supported CallList Replay mode" };

	while (TRUE)
	{
		// No welcome screen, because no touch, TODO: Add a timer to the welcome screen
		// WelcomeScreen(s_pHalContext, info);

		EW2023_CarDashBoard();

		EVE_Util_clearScreen(s_pHalContext);

		EVE_Hal_close(s_pHalContext);
		EVE_Hal_release();

		/* Init HW Hal for next loop*/
		Gpu_Init(s_pHalContext);
	}
	return 0;
}

void Input_To_Flash() {
	// The 1440x540 has no touch
	// Do flasing if user touch keyboard on the console
	int sent = 1;
#ifdef FT9XX_PLATFORM
#define FLASH_INIT_STATUP 1
#endif
#ifdef FLASH_INIT_STATUP
	EVE_Util_loadSdCard(s_pHalContext);
	sent = Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "EW2023_CarDashBoard.bin", "EW2023_CarDashBoard.bin", 0);
#elif defined(RP2040_PLATFORM)

	MSG("Press anykey to flashing from SD card ...");
	MSG("Type r to reboot");
	MSG("Type u to reopen USB drive");

	const uint32_t waiting = 5000;
	uint32_t timeStartMs = EVE_millis();
	uint32_t dur = 0;
	uint32_t count = waiting / 1000;
	uint32_t count_last = count;
	while (dur < waiting) {
		dur = min(EVE_millis() - timeStartMs, waiting);
		char c = EVE_Ext_GetChar();
		if (c) {
			EVE_Util_loadSdCard(s_pHalContext);
			sent = Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "EW2023_CarDashBoard.bin", "EW2023_CarDashBoard.bin", 0);
			break;
		}
		count = (waiting - dur) / 1000;
		if (count != count_last) {
			count_last = count;
			printf("..%us\r\n", count);
			Draw_Text_Format2(phost, 1, "Press anykey to flash from SD card (%u second)", count);
		}
	}
	printf("\r\n");
#endif

	/// If fail to program flash, pause program
	if (0 >= sent) {
		while (1) {
			MSG("Failed to read SD card");
			EVE_sleep(1000);
		}
	}
}
// ------ Application ---------------------------------------------------------

void serial_replay_task();
int serial_replay_task_init();

#if !defined(ENABLE_RECORD_CALLIST)
#define ENABLE_RECORD_CALLIST 0
#endif
#if !defined(ENABLE_REPLAY_CALLIST)
#ifndef FT9XX_PLATFORM // Not enough memory for malloc on ft9xx
#define ENABLE_REPLAY_CALLIST 1
#endif
#endif

#define START_X_OFFSET     (325)
#define LCD_DISPLAY_WIDTH  (1440)
#define LCD_DISPLAY_HEIGHT (540)

#define N_SPEED (0)
#define N_RPM (1)
#define N_FUEL (2)
#define N_TEMP (3)

SystemStatus_t SystemStatus;
CarInfo carinfo;
float NeedleDeaccelaration[4] = { 0 }; /* de-accelaration the needle movement: speed, rpm, temp, fuel */
float old_temperature = 50;

Image_header_t image_item[] = {
	{COMPRESSED_RGBA_ASTC_6x6_KHR, LCD_DISPLAY_WIDTH, LCD_DISPLAY_HEIGHT, LCD_DISPLAY_WIDTH, 0},			  // Background 1 - From RAM_G
	{COMPRESSED_RGBA_ASTC_4x4_KHR, 40, 40, 40, 345600},														  // Gear hightligh box
	{COMPRESSED_RGBA_ASTC_4x4_KHR, 12, 160, 12, 345600 + 1600},												  // needle of speed
	{COMPRESSED_RGBA_ASTC_4x4_KHR, LCD_DISPLAY_WIDTH / 2, LCD_DISPLAY_HEIGHT, LCD_DISPLAY_WIDTH, 0},		  // Background 2 - Left half
	{COMPRESSED_RGBA_ASTC_4x4_KHR, LCD_DISPLAY_WIDTH / 2, LCD_DISPLAY_HEIGHT, LCD_DISPLAY_WIDTH, 0 + 388800}, // Background 2 - Right half
	{COMPRESSED_RGBA_ASTC_4x4_KHR, 20, 212, 20 * 2, 0 + 388800 + 388800},									  // needle of speed and RPM
};

#include "EW2023_CarDashBoard_CallList_Record.c"
#include "EW2023_CarDashBoard_CallList_Replay.c"

void Setup_Bitmap_New(uint8_t bIndex, uint8_t handle, float scale)
{
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(handle));
	Gpu_CoCmd_SetBitmap(phost, image_item[bIndex].Arrayoffset, image_item[bIndex].Format, image_item[bIndex].Width, image_item[bIndex].Height);
	if ((scale != 1) && (scale != 0))
	{
		App_WrCoCmd_Buffer(phost, BITMAP_TRANSFORM_A((uint16_t)(256L * scale)));
		App_WrCoCmd_Buffer(phost, BITMAP_TRANSFORM_E((uint16_t)(256L * scale)));
	}
}

void rotate_draw_image(uint8_t handle, float rotateAngle, int16_t x, int16_t y, uint16_t x_center, uint16_t y_center, int16_t x_translate_offset, int16_t y_translate_offset)
{
	App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((handle)));
#define FX16(x) ((long)(65536L * x))
	Gpu_CoCmd_LoadIdentity(phost);
	Gpu_CoCmd_Translate(phost, FX16(x_translate_offset), FX16(y_translate_offset));
	Gpu_CoCmd_Rotate(phost, rotateAngle);
	Gpu_CoCmd_Translate(phost, FX16(-x_center), FX16(-y_center));
	Gpu_CoCmd_SetMatrix(phost);
	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(3));
	App_WrCoCmd_Buffer(phost, VERTEX2F((long)((x - x_translate_offset) * 8), (long)((y - y_translate_offset) * 8)));
	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(4));
	App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());
}

void Draw_Bitmap(uint8_t handle, int16_t x, int16_t y)
{
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((handle)));
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(3));
	App_WrCoCmd_Buffer(phost, VERTEX2F(8 * x, 8 * y));
	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(4));
	App_WrCoCmd_Buffer(phost, END());
}

void Draw_GUI_1()
{
	/* draw gear box*/
	Draw_Bitmap(HANDLE_GUI_1_BACKGROUND, 0, 0);

	switch (SystemStatus.gear)
	{
	case 'D':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720, 416); //"D"
		break;
	case '1':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720 + 36, 416); //"1"
		break;
	case '2':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720 + 36 + 36, 416); //"2"
		break;
	case '3':
	case '4':
	case '5':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720 + 36 + 36 + 36, 416); //"3"
		break;
	case 'P':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720 - 36 - 36 - 36, 416); //"P"
		break;
	case 'R':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720 - 36 - 36, 416); //"R"
		break;
	case 'N':
	default:
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, 720 - 36, 416); //"N"
	}

	/* Draw needle */
	float angle;
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((HANDLE_GUI_1_NEEDLE)));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 400, 400));

	const int16_t x_center = 5;
	const int16_t y_center = 21;
	const int16_t x_translate_offset = 212, y_translate_offset = 212;

	int16_t Init_Angle = 30;   // The needle angle when speed is 0.
	int16_t Speed_Range = 160; // from 0 to 160 km
	int16_t Angle_Range = 265; // The needel angle range from 0 to 160 km

	angle = SystemStatus.speed / Speed_Range * Angle_Range + Init_Angle;

	// The angle step is different when speed is over 160, handle it specially
	const int16_t SPEED_BASE = 160; // 160 kmh
	if (SystemStatus.speed >= SPEED_BASE)
	{
		Init_Angle += Angle_Range; // The init angle is set to the position of SPEED_BASE

		Angle_Range = 17;
		Speed_Range = 20;

		angle = (SystemStatus.speed - SPEED_BASE) / Speed_Range * Angle_Range + Init_Angle;
	}

	DEACCELARATION(angle, NeedleDeaccelaration[N_SPEED], 0.04);

	int32_t rotateAngle = (int32_t)((angle) * 65536 / 360);
	rotate_draw_image(HANDLE_GUI_1_NEEDLE, rotateAngle, 733, 249, x_center, y_center, x_translate_offset, y_translate_offset); // Speed needle
}

void Draw_GUI_2()
{
	const int16_t x_center = 9;
	const int16_t y_center = 51;
	const int16_t x_translate_offset = 212, y_translate_offset = 212;

	float angle; // Rotating angle of needle
	int32_t rotateAngle;

	Draw_Bitmap(HANDLE_GUI_2_BACKGROUND_LEFTHALF, 0, 0);

	Draw_Bitmap(HANDLE_GUI_2_BACKGROUND_RIGHTHALF, LCD_DISPLAY_WIDTH / 2, 0);

	/* Draw needle */
	const int16_t MAX_RPM = (600L);
	const int16_t RPM_NEEDLE_RANGE = (300); // 300 degree from min speed 0 to max 6000
	angle = SystemStatus.rpm * (RPM_NEEDLE_RANGE) / MAX_RPM;
	DEACCELARATION(angle, NeedleDeaccelaration[N_RPM], 0.04);

	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((HANDLE_GUI_2_NEEDLE)));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 424, 424));

	rotateAngle = (int32_t)((angle) * 65536 / 360);
	rotate_draw_image(HANDLE_GUI_2_NEEDLE, rotateAngle, 341, 301, x_center, y_center, x_translate_offset, y_translate_offset); // RPM Needle

	const int16_t BASE_SPEED_RANGE = 20; // from 0 to 20 MPH using different step

	int16_t SPEED_RANGE = (180 - BASE_SPEED_RANGE); // in MPH
	int16_t SPEED_NEEDLE_RANGE = (240);				// 300 degree from min speed 20 to max 180 MPH
	int16_t INIT_ANGLE = 60;

	angle = (SystemStatus.speed - BASE_SPEED_RANGE) / (SPEED_RANGE) * (SPEED_NEEDLE_RANGE)+INIT_ANGLE;
	if (SystemStatus.speed <= BASE_SPEED_RANGE)
	{
		SPEED_RANGE = (BASE_SPEED_RANGE); // in MPH
		SPEED_NEEDLE_RANGE = (60);		  // 300 degree from min speed 0 to max 180 MPH
		INIT_ANGLE = 0;
		angle = SystemStatus.speed / (SPEED_RANGE) * (SPEED_NEEDLE_RANGE)+INIT_ANGLE;
	}

	DEACCELARATION(angle, NeedleDeaccelaration[N_SPEED], 0.04);
	rotateAngle = (int32_t)((angle) * 65536 / 360);
	// printf("rotateAngle = %d\n",rotateAngle);
	rotate_draw_image(HANDLE_GUI_2_NEEDLE, rotateAngle, 1098, 301, x_center, y_center, x_translate_offset, y_translate_offset); // Speed needle
}

static void Swap_UI() {
	printf("swap ui\r\n");
	/*switch the GUI mode when the button is released*/
	SystemStatus.picIndex++;
	SystemStatus.screen_status = SCREEN_INIT;
	if (SystemStatus.picIndex >= NUM_BACKGROUND)
		SystemStatus.picIndex = 0;

	/* Load dummy screen for smoother transition between GUIs*/
	App_WrCoCmd_Buffer(phost, CMD_DLSTART);
	App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
	App_WrCoCmd_Buffer(phost, DISPLAY());
	Gpu_CoCmd_Swap(phost);
	App_Flush_Co_Buffer(phost);
	Gpu_Hal_WaitCmdfifo_empty(phost);

}

/**
* Swap UI when user press an external button or on console
*/
void Event_ExtBtn_Press()
{
#if defined(_WIN32)
	if (EVE_Ext_GetChar() != 0) {
		Swap_UI();
	}
	return;
#endif
#if defined(EMBEDDED_PLATFORM)
	if (EVE_Ext_GetChar() != 0) {
#if defined(_DEBUG)
		Swap_UI();
		return;
#endif
	}

	// trigger when button released
	static bool saveLastBtnState = 0;
	bool isBtnReleased = 0;
	bool btnState = READ_PUSH_BUTTON;

	if (btnState == 1) {
		saveLastBtnState = 1;
		isBtnReleased = 0;
	}
	else if (saveLastBtnState == 1) {
		isBtnReleased = 1;
	}

	if (isBtnReleased)
	{
		// Swap UI once
		Swap_UI();
		// reset
		saveLastBtnState = 0;
	}
#endif
}


void Update_SystemStatus()
{
	float temp;

	serial_replay_task();

	/* Simulate temperature (from 0% to 100%)*/
	temp = SystemStatus.rpm / 8;
	DEACCELARATION(temp, old_temperature, 0.001);
	SystemStatus.temperature = (int)temp;
}

char* getFPS() {
	static uint32_t sms = 0;
	static uint32_t ems = 0;
	static int count = 0;
	static char fr[100] = "Measuring FPS...";

	if (count == 0) {
		sms = EVE_millis();
	}
	count++;
	if (count == 15) {
		ems = EVE_millis();
		float measuredFramerate = 1000.0 * count / (ems - sms);
		snprintf(fr, sizeof(fr), "FPS = %.1f", measuredFramerate);
		count = 0;
		sms = EVE_millis();
	}
	// printf("%s\n", fr);
	return fr;
}

void EW2023_CarDashBoard()
{
#if defined(FT9XX_PLATFORM)
	while(!serial_replay_task_init()){
		EVE_sleep(1000);
		APP_ERR("Cannot read data file");
	}
#endif
#if defined(RP2040_PLATFORM)
	gpio_set_function(RP2040_GPIO_PUSHBUTTON, GPIO_FUNC_NULL);
	gpio_set_dir(RP2040_GPIO_PUSHBUTTON, false);
	gpio_pull_down(RP2040_GPIO_PUSHBUTTON);
#endif

#if ENABLE_RECORD_CALLIST
	Draw_Text(phost, "Recording Car DashBoard demo with callist");
	EW2023_CarDashBoard_CallList_Record();
#endif
#if ENABLE_REPLAY_CALLIST
	Draw_Text(phost, "Replaying Car DashBoard demo with callist");
	EW2023_CarDashBoard_CallList_Replay();
#endif

	uint32_t frametime = 0;
	uint32_t tmp;

	FlashHelper_SwitchFullMode(phost);

	/* Setup initial system */
	SystemStatus.rpm = 0;
	SystemStatus.speed = 0;
	SystemStatus.temperature = 50;

	SystemStatus.screen_status = SCREEN_INIT;
	SystemStatus.picIndex = 0;

	App_Flush_Co_Buffer(phost);
	Gpu_Hal_WaitCmdfifo_empty(phost);

	EVE_CoCmd_setRotate(s_pHalContext, 1);// bottom up

	do
	{
		App_WrCoCmd_Buffer(phost, CMD_DLSTART);
		App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(125, 125, 100));
		App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
		App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

		if (SystemStatus.screen_status == SCREEN_INIT && SystemStatus.picIndex == 0)
		{
			uint32_t flashAddr = 4096, guiSize = 1024;
			flashAddr = 785984;
			guiSize = 345600 + 1600 + 1920; // Add up all the assets
			Gpu_CoCmd_FlashRead(phost, RAM_G, flashAddr, guiSize);

			Setup_Bitmap_New(IMAGE_GUI_1_BACKGROUND, HANDLE_GUI_1_BACKGROUND, 1);
			Setup_Bitmap_New(IMAGE_GUI_1_GEAR_BOX, HANDLE_GUI_1_GERA_BOX, 1);
			Setup_Bitmap_New(IMAGE_GUI_1_NEEDLE, HANDLE_GUI_1_NEEDLE, 1);
			Gpu_Hal_WaitCmdfifo_empty(phost);
			SystemStatus.screen_status = SCREEN_MAIN;
		}
		else if (SystemStatus.screen_status == SCREEN_INIT && SystemStatus.picIndex == 1)
		{
			uint32_t flashAddr = 4096, guiSize = 1024;
			flashAddr = 4096;
			guiSize = 388800 * 2 + 4288;
			Gpu_CoCmd_FlashRead(phost, RAM_G, flashAddr, guiSize);
			Setup_Bitmap_New(IMAGE_GUI_2_BACKGROUND_LEFTHALF, HANDLE_GUI_2_BACKGROUND_LEFTHALF, 1);
			Setup_Bitmap_New(IMAGE_GUI_2_BACKGROUND_RIGHTHALF, HANDLE_GUI_2_BACKGROUND_RIGHTHALF, 1);
			Setup_Bitmap_New(IMAGE_GUI_2_NEEDLE, HANDLE_GUI_2_NEEDLE, 1);
			Gpu_Hal_WaitCmdfifo_empty(phost);
			SystemStatus.screen_status = SCREEN_MAIN;
		}
		else if (SystemStatus.screen_status == SCREEN_MAIN && SystemStatus.picIndex == 0)
		{
			Draw_GUI_1();
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
			EVE_CoCmd_text(phost, phost->Width - 200, 60, 28, 0, getFPS());
		}
		else if (SystemStatus.screen_status == SCREEN_MAIN && SystemStatus.picIndex == 1)
		{
			Draw_GUI_2();
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
			EVE_CoCmd_text(phost, phost->Width - 200, 60, 28, 0, getFPS());
		}

		App_WrCoCmd_Buffer(phost, DISPLAY());
		EVE_CoCmd_swap(phost);
		EVE_Cmd_waitFlush(phost);
		Event_ExtBtn_Press();
		Update_SystemStatus(); /* Change car status: speed, rpm, temperature, pressure, energy ... */
		tmp = EVE_millis(); // calculate by msvc
		frametime = tmp;
	} while (1);
}

#if SERIAL_ENABLE
uint8_t header = 0;
char opt, par;

char DEVICE_NAME[] = "SimHub Dash";
uint8_t read_bytes;
#ifdef DEBUG_SERIAL_ENABLED
char Debug[255];
#endif
void serial_task()
{
	char data[256];
	static uint8_t first_setup = 1;
	static count = 0;
	static uint16_t lost_count = 0;
	static br = BAUD_19200; // 19200 baudrate
	serialRead();
	read_bytes = readData(data);
	{
		static int t = 0;
		if (t++ > 3)
		{
			t = 0;
			uart1_printf("s");
			uart0_printf("s");
		}
		return;
	}

	count++;
	if (read_bytes >= 2)
	{
		uart0_printf("read_bytes=%d", read_bytes);
		char header, opt;
		header = data[0];
		opt = data[1];
#ifdef DEBUG_SERIAL_ENABLED
		memcpy(Debug, data, read_bytes);
		Debug[read_bytes] = '\0';
		uart0_printf("%d %s -\n", count, Debug);
#endif
		count = 0;

		if ((header == MESSAGE_HEADER))
		{
			// Hello command
			if (opt == '1')
			{
				// first_setup = 1;
				EVE_sleep(10);
				serialWrite('h');
				serialFlush();
			}

			// Set baudrate
			if (opt == '8')
			{
				if (read_bytes >= 3)
				{
					// printf("---- RESET BAUD RATE TO 115200 ---\n");
				}
				serialWriteRaw(0x01);
				serialFlush();
			}

			// Simple buttons count
			if (opt == 'J')
			{
				serialWrite((uint8_t)0x00);
				serialFlush();
			}

			//  Module count command
			if (opt == '2')
			{
				serialWrite((uint8_t)0x00);
				serialFlush();
			}

			//  SIMPLE Module count command
			if (opt == 'B')
			{
				serialWrite((uint8_t)0x00);
				serialFlush();
			}

			// ACQ Packet
			if (opt == 'A')
			{
				serialWrite(0x03);
				serialFlush();
			}

			// Buttons state
			if (opt == 'C')
			{
				// sendButtonState();
				// serialFlush();
			}

			// Device Name
			if (opt == 'N')
			{
				serialWriteString(DEVICE_NAME, strlen(DEVICE_NAME));
				serialFlush();
			}
			//  RGBLED count command
			if (opt == '4')
			{
				serialWrite(0x00);
				serialFlush();
			}
			// Tachometer
			if (opt == 'T')
			{
			}

			// Boost gauge
			if (opt == 'U')
			{
			}

			// Features command
			if (opt == '0')
			{

				// Gear
				serialWrite('G');

				// Name
				serialWrite('N');

				// Additional buttons
				// serialWrite('J');

				// Custom Protocol Support
				serialWrite('P');

				serialWrite(0x0a); // Finish command
				serialFlush();
			}

			// GEAR
			if (opt == 'G')
			{
				// char gear = FlowSerialTimedRead();
				serialWriteRaw((char)0x01);
				serialFlush();
			}

			// Custom protocol for end users (See Wiki for more infos : https://github.com/zegreatclan/AssettoCorsaTools/wiki/Custom-Arduino-hardware-support)
			if (opt == 'P')
			{
				serialWriteRaw((char)0x01);
				serialFlush();
				Get_GameInfo(data, read_bytes);
			}
		}
		else
		{
			// serialResponse();
		}

		lost_count = 0;
	}

	else
	{ // If do not receive UART signal for a while, then we may lost connection
		// Reset the UART speed to 19200
		lost_count++;
		if ((lost_count > 100))
		{
			br = BAUD_19200;
			serialSetBaudrate(br); // Set baudrate = 19200
			lost_count = 0;
			// printf("---- RESET BAUD RATE TO 19200 ---\n");
		}
	}
	serialResponse();
}

#endif

/* Nothing beyond this */
