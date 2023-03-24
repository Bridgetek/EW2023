/**
 * @file EW2023_CarDashBoard_CallList_Replay.c
 * @brief Car DashBoard demo using CallList
 *
 * @author Bridgetek
 *
 * @date 2022
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
#if defined(EVE_EXT_CALLLIST_ENABLE)
#define EW2023_CARDASHBOARD_CALLLIST_Replay 1
char* getFPS();
void Event_ExtBtn_Press();

void rotate_draw_image_CallList_Replay(t_EVE_Ext_CallList* calllist, int nth, uint8_t handle, float rotateAngle, int16_t x, int16_t y, uint16_t x_center, uint16_t y_center, int16_t x_translate_offset, int16_t y_translate_offset)
{
	uint32_t data[2] = { 0 };
	data[0] = FX16(x_translate_offset);
	data[1] = FX16(y_translate_offset);
	EVE_Ext_CallList_Replay_Entry(phost, calllist, nth++, data);
	data[0] = rotateAngle;
	EVE_Ext_CallList_Replay_Entry(phost, calllist, nth++, data);
	data[0] = FX16(-x_center);
	data[1] = FX16(-y_center);
	EVE_Ext_CallList_Replay_Entry(phost, calllist, nth++, data);
	data[0] = VERTEX2F((long)((x - x_translate_offset) * 8), (long)((y - y_translate_offset) * 8));
	EVE_Ext_CallList_Replay_Entry(phost, calllist, nth++, data);
}

void Draw_Bitmap_CallList_Replay(t_EVE_Ext_CallList* calllist, int nth, uint8_t handle, int16_t x, int16_t y)
{
	uint32_t data[1] = { 0 };
	data[0] = VERTEX2F(8 * x, 8 * y);
	EVE_Ext_CallList_Replay_Entry(phost, calllist, nth, data);
}

void Draw_GUI_1_CallList_Replay(t_EVE_Ext_CallList* calllist)
{
	/* draw gear box*/
	Draw_Bitmap_CallList_Replay(calllist, 0, HANDLE_GUI_1_BACKGROUND, 0, 0);

	switch (SystemStatus.gear)
	{
	case 'D':
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720, 416); //"D"
		break;
	case '1':
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720 + 36, 416); //"1"
		break;
	case '2':
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720 + 36 + 36, 416); //"2"
		break;
	case '3':
	case '4':
	case '5':
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720 + 36 + 36 + 36, 416); //"3"
		break;
	case 'P':
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720 - 36 - 36 - 36, 416); //"P"
		break;
	case 'R':
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720 - 36 - 36, 416); //"R"
		break;
	case 'N':
	default:
		Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_1_GERA_BOX, 720 - 36, 416); //"N"
	}

	/* Draw needle */
	float angle;

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
	rotate_draw_image_CallList_Replay(calllist, 2, HANDLE_GUI_1_NEEDLE, rotateAngle, 733, 249, x_center, y_center, x_translate_offset, y_translate_offset); // Speed needle
}

void Draw_GUI_2_CallList_Replay(t_EVE_Ext_CallList* calllist)
{
	const int16_t x_center = 9;
	const int16_t y_center = 51;
	const int16_t x_translate_offset = 212, y_translate_offset = 212;

	float angle; // Rotating angle of needle
	int32_t rotateAngle;

	Draw_Bitmap_CallList_Replay(calllist, 0, HANDLE_GUI_2_BACKGROUND_LEFTHALF, 0, 0);

	Draw_Bitmap_CallList_Replay(calllist, 1, HANDLE_GUI_2_BACKGROUND_RIGHTHALF, LCD_DISPLAY_WIDTH / 2, 0);

	/* Draw needle */
	const int16_t MAX_RPM = (600L);
	const int16_t RPM_NEEDLE_RANGE = (300); // 300 degree from min speed 0 to max 6000
	angle = SystemStatus.rpm * (RPM_NEEDLE_RANGE) / MAX_RPM;
	DEACCELARATION(angle, NeedleDeaccelaration[N_RPM], 0.04);

	rotateAngle = (int32_t)((angle) * 65536 / 360);
	rotate_draw_image_CallList_Replay(calllist, 2, HANDLE_GUI_2_NEEDLE, rotateAngle, 341, 301, x_center, y_center, x_translate_offset, y_translate_offset); // RPM Needle

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
	rotate_draw_image_CallList_Replay(calllist, 6, HANDLE_GUI_2_NEEDLE, rotateAngle, 1098, 301, x_center, y_center, x_translate_offset, y_translate_offset); // Speed needle
}

void EW2023_CarDashBoard_CallList_Replay()
{
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

	uint32_t callListOffset = 388800 * 2 + 4288;
	t_EVE_Ext_CallList* callListUI1Setup = EVE_Ext_CallList_Instance_Init_Exising(phost, callListOffset, TEST_DIR "/EW2023_CarDashBoard_callListUI1Setup.bin");
	t_EVE_Ext_CallList* callListUI1Draw = EVE_Ext_CallList_Instance_Init_Exising(phost, callListOffset, TEST_DIR "/EW2023_CarDashBoard_callListUI1Draw.bin");
	t_EVE_Ext_CallList* callListUI2Setup = EVE_Ext_CallList_Instance_Init_Exising(phost, callListOffset, TEST_DIR "/EW2023_CarDashBoard_callListUI2Setup.bin");
	t_EVE_Ext_CallList* callListUI2Draw = EVE_Ext_CallList_Instance_Init_Exising(phost, callListOffset, TEST_DIR "/EW2023_CarDashBoard_callListUI2Draw.bin");

	do
	{
		if (SystemStatus.screen_status == SCREEN_INIT && SystemStatus.picIndex == 0)
		{
			EVE_Ext_CallList_Replay_Start(phost, callListUI1Setup);
			EVE_Ext_CallList_Replay_Start(phost, callListUI1Draw);
			SystemStatus.screen_status = SCREEN_MAIN;
		}
		else if (SystemStatus.screen_status == SCREEN_INIT && SystemStatus.picIndex == 1)
		{
			EVE_Ext_CallList_Replay_Start(phost, callListUI2Setup);
			EVE_Ext_CallList_Replay_Start(phost, callListUI2Draw);
			SystemStatus.screen_status = SCREEN_MAIN;
		}
		else if (SystemStatus.screen_status == SCREEN_MAIN && SystemStatus.picIndex == 0)
		{
			Draw_GUI_1_CallList_Replay(callListUI1Draw);
			EVE_Ext_CallList_Replay_Entry_SPACE(phost, callListUI1Draw, 6, getFPS());
			// debug
			// Ftf_Read_File_From_RAM_G(phost, TEST_DIR "/u2aftercmdtext.bin", callListUI1Draw->ramGOffset, 500);
			EVE_Ext_CallList_Replay_Repeat(phost, callListUI1Draw);
		}
		else if (SystemStatus.screen_status == SCREEN_MAIN && SystemStatus.picIndex == 1)
		{
			Draw_GUI_2_CallList_Replay(callListUI2Draw);
			EVE_Ext_CallList_Replay_Entry_SPACE(phost, callListUI2Draw, 10, getFPS());
			EVE_Ext_CallList_Replay_Repeat(phost, callListUI2Draw);				
		}

		Event_ExtBtn_Press();
		Update_SystemStatus(); /* Change car status: speed, rpm, temperature, pressure, energy ... */
		tmp = EVE_millis(); // calculate by msvc
		frametime = tmp;
	} while (1);
}

#endif // EVE_EXT_CALLLIST_ENABLE
/* Nothing beyond this */
