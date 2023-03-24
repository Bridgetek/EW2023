/**
 * @file EW2023_CarDashBoard_CallList_Record.c
 * @brief Record the CallList of Car DashBoard demo
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
#if defined(EVE_EXT_CALLLIST_ENABLE)
#define EW2023_CARDASHBOARD_CALLLIST_Record 1
void Setup_Bitmap_New(uint8_t bIndex, uint8_t handle, float scale);
void Event_ExtBtn_Press();
void Update_SystemStatus();

void rotate_draw_image_CallList_Record(t_EVE_Ext_CallList* calllist, uint8_t handle, float rotateAngle, int16_t x, int16_t y, uint16_t x_center, uint16_t y_center, int16_t x_translate_offset, int16_t y_translate_offset)
{
	App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((handle)));

#define FX16(x) ((long)(65536L * x))
	Gpu_CoCmd_LoadIdentity(phost);

	// entry 1st
	EVE_Ext_CallList_Record_Entry_CoCmd(phost, calllist, sizeof(uint32_t) *2);
	Gpu_CoCmd_Translate(phost, FX16(x_translate_offset), FX16(y_translate_offset));

	EVE_Ext_CallList_Record_Entry_CoCmd(phost, calllist, sizeof(uint32_t));
	Gpu_CoCmd_Rotate(phost, rotateAngle);

	EVE_Ext_CallList_Record_Entry_CoCmd(phost, calllist, sizeof(uint32_t) * 2);
	Gpu_CoCmd_Translate(phost, FX16(-x_center), FX16(-y_center));

	Gpu_CoCmd_SetMatrix(phost);
	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(3));

	EVE_Ext_CallList_Record_Entry_DlCmd(phost, calllist);
	App_WrCoCmd_Buffer(phost, VERTEX2F((long)((x - x_translate_offset) * 8), (long)((y - y_translate_offset) * 8)));

	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(4));
	App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());
}

void Draw_Bitmap_CallList_Record(t_EVE_Ext_CallList* calllist, uint8_t handle, int16_t x, int16_t y)
{
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((handle)));

	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(3));

	EVE_Ext_CallList_Record_Entry_DlCmd(phost, calllist);
	App_WrCoCmd_Buffer(phost, VERTEX2F(8 * x, 8 * y));

	App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(4));
	App_WrCoCmd_Buffer(phost, END());
}

void Draw_GUI_1_CallList_Record(t_EVE_Ext_CallList* calllist)
{
	/* draw gear box*/
	Draw_Bitmap_CallList_Record(calllist, HANDLE_GUI_1_BACKGROUND, 0, 0);
	Draw_Bitmap_CallList_Record(calllist, HANDLE_GUI_1_GERA_BOX, 720, 416); //"D"
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((HANDLE_GUI_1_NEEDLE)));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 400, 400));
	rotate_draw_image_CallList_Record(calllist, HANDLE_GUI_1_NEEDLE, 0, 733, 249, 0, 0, 0, 0); // Speed needle
}

void Draw_GUI_2_CallList_Record(t_EVE_Ext_CallList* calllist)
{
	Draw_Bitmap_CallList_Record(calllist, HANDLE_GUI_2_BACKGROUND_LEFTHALF, 0, 0);
	Draw_Bitmap_CallList_Record(calllist, HANDLE_GUI_2_BACKGROUND_RIGHTHALF, LCD_DISPLAY_WIDTH / 2, 0);
	App_WrCoCmd_Buffer(phost, BITMAP_HANDLE((HANDLE_GUI_2_NEEDLE)));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 424, 424));
	rotate_draw_image_CallList_Record(calllist, HANDLE_GUI_2_NEEDLE, 0, 733, 249, 0, 0, 0, 0); // Speed needle
	rotate_draw_image_CallList_Record(calllist, HANDLE_GUI_2_NEEDLE, 0, 733, 249, 0, 0, 0, 0); // Speed needle
}

void EW2023_CarDashBoard_CallList_Record()
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
	t_EVE_Ext_CallList* callListUI1Setup = EVE_Ext_CallList_Instance_Init(phost, callListOffset, TEST_DIR "/CarDashBoard_callListUI1Setup.bin");
	t_EVE_Ext_CallList* callListUI1Draw = EVE_Ext_CallList_Instance_Init(phost, callListOffset, TEST_DIR "/CarDashBoard_callListUI1Draw.bin");
	t_EVE_Ext_CallList* callListUI2Setup = EVE_Ext_CallList_Instance_Init(phost, callListOffset, TEST_DIR "/CarDashBoard_callListUI2Setup.bin");
	t_EVE_Ext_CallList* callListUI2Draw = EVE_Ext_CallList_Instance_Init(phost, callListOffset, TEST_DIR "/CarDashBoard_callListUI2Draw.bin");

	do
	{
		if (!callListUI1Setup && !callListUI1Draw && !callListUI2Setup && !callListUI2Draw) {
			break;;
		}

		if (SystemStatus.screen_status == SCREEN_INIT && SystemStatus.picIndex == 0)
		{
			if (callListUI1Setup)
			{
				Draw_Text_Format(phost, "Recoring Callist: %s", callListUI1Setup->path_bin_file);
				EVE_Ext_CallList_Record_Start(phost, callListUI1Setup, callListOffset);
			}


			App_WrCoCmd_Buffer(phost, CMD_DLSTART);
			App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(125, 125, 100));
			App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

			uint32_t flashAddr = 4096, guiSize = 1024;
			flashAddr = 785984;
			guiSize = 345600 + 1600 + 1920; // Add up all the assets
			Gpu_CoCmd_FlashRead(phost, RAM_G, flashAddr, guiSize);

			Setup_Bitmap_New(IMAGE_GUI_1_BACKGROUND, HANDLE_GUI_1_BACKGROUND, 1);
			Setup_Bitmap_New(IMAGE_GUI_1_GEAR_BOX, HANDLE_GUI_1_GERA_BOX, 1);
			Setup_Bitmap_New(IMAGE_GUI_1_NEEDLE, HANDLE_GUI_1_NEEDLE, 1);
			Gpu_Hal_WaitCmdfifo_empty(phost);
			SystemStatus.screen_status = SCREEN_MAIN;

			App_WrCoCmd_Buffer(phost, DISPLAY());
			EVE_CoCmd_swap(phost);

			// saving callist to file
			if (callListUI1Setup) {
				// Record_Stop must be call before EVE_Cmd_waitFlush, or the co-processor hang up
				EVE_Ext_CallList_Record_Stop(phost);
				EVE_Ext_CallList_Instance_Save(phost, callListUI1Setup);
				EVE_Ext_CallList_Instance_DeInit(&callListUI1Setup);
			}
		}
		else if (SystemStatus.screen_status == SCREEN_INIT && SystemStatus.picIndex == 1)
		{
			if (callListUI2Setup) {
				Draw_Text_Format(phost, "Recoring Callist: %s", callListUI2Setup->path_bin_file);
				EVE_Ext_CallList_Record_Start(phost, callListUI2Setup, callListOffset);
			}

			App_WrCoCmd_Buffer(phost, CMD_DLSTART);
			App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(125, 125, 100));
			App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

			uint32_t flashAddr = 4096, guiSize = 1024;
			flashAddr = 4096;
			guiSize = 388800 * 2 + 4288;
			Gpu_CoCmd_FlashRead(phost, RAM_G, flashAddr, guiSize);
			Setup_Bitmap_New(IMAGE_GUI_2_BACKGROUND_LEFTHALF, HANDLE_GUI_2_BACKGROUND_LEFTHALF, 1);
			Setup_Bitmap_New(IMAGE_GUI_2_BACKGROUND_RIGHTHALF, HANDLE_GUI_2_BACKGROUND_RIGHTHALF, 1);
			Setup_Bitmap_New(IMAGE_GUI_2_NEEDLE, HANDLE_GUI_2_NEEDLE, 1);
			Gpu_Hal_WaitCmdfifo_empty(phost);
			SystemStatus.screen_status = SCREEN_MAIN;

			App_WrCoCmd_Buffer(phost, DISPLAY());
			EVE_CoCmd_swap(phost);

			// saving callist to file
			if (callListUI2Setup) {
				// Record_Stop must be call before EVE_Cmd_waitFlush, or the co-processor hang up
				EVE_Ext_CallList_Record_Stop(phost);
				EVE_Ext_CallList_Instance_Save(phost, callListUI2Setup);
				EVE_Ext_CallList_Instance_DeInit(&callListUI2Setup);
			}
		}
		else if (SystemStatus.screen_status == SCREEN_MAIN && SystemStatus.picIndex == 0)
		{
			if (callListUI1Draw) {
				Draw_Text_Format(phost, "Recoring Callist: %s", callListUI1Draw->path_bin_file);
				EVE_Ext_CallList_Record_Start(phost, callListUI1Draw, callListOffset);
			}
			App_WrCoCmd_Buffer(phost, CMD_DLSTART);
			App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(125, 125, 100));
			App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

			Draw_GUI_1_CallList_Record(callListUI1Draw);
#define MAX_TEXT 40
			char text40[MAX_TEXT];
			memset(text40, ' ', MAX_TEXT);
			text40[MAX_TEXT - 1] = 0;
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
			EVE_Ext_CallList_Record_Entry(phost, callListUI1Draw, 12, sizeof(uint8_t) * MAX_TEXT);
			EVE_CoCmd_text(phost, phost->Width - 200, 10, 28, 0, text40);

			App_WrCoCmd_Buffer(phost, DISPLAY());
			EVE_CoCmd_swap(phost);

			// saving callist to file
			if (callListUI1Draw) {
				// Record_Stop must be call before EVE_Cmd_waitFlush, or the co-processor hang up
				EVE_Ext_CallList_Record_Stop(phost);
				EVE_Ext_CallList_Instance_Save(phost, callListUI1Draw);
				EVE_Ext_CallList_Instance_DeInit(&callListUI1Draw);
			}
		}
		else if (SystemStatus.screen_status == SCREEN_MAIN && SystemStatus.picIndex == 1)
		{
			if (callListUI2Draw) {
				Draw_Text_Format(phost, "Recoring Callist: %s", callListUI2Draw->path_bin_file);
				EVE_Ext_CallList_Record_Start(phost, callListUI2Draw, callListOffset);
			}
			App_WrCoCmd_Buffer(phost, CMD_DLSTART);
			App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(125, 125, 100));
			App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

			Draw_GUI_2_CallList_Record(callListUI2Draw);
#define MAX_TEXT 40
			char text40[MAX_TEXT];
			memset(text40, ' ', MAX_TEXT);
			text40[MAX_TEXT - 1] = 0;
			App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
			EVE_Ext_CallList_Record_Entry(phost, callListUI2Draw, 12, sizeof(uint8_t) * MAX_TEXT);
			EVE_CoCmd_text(phost, phost->Width - 200, 10, 28, 0, text40);

			App_WrCoCmd_Buffer(phost, DISPLAY());
			EVE_CoCmd_swap(phost);

			// saving callist to file
			if (callListUI2Draw) {
				// Record_Stop must be call before EVE_Cmd_waitFlush, or the co-processor hang up
				EVE_Ext_CallList_Record_Stop(phost);
				EVE_Ext_CallList_Instance_Save(phost, callListUI2Draw);
				EVE_Ext_CallList_Instance_DeInit(&callListUI2Draw);
			}
		}

		Event_ExtBtn_Press();
		Update_SystemStatus(); /* Change car status: speed, rpm, temperature, pressure, energy ... */
		tmp = EVE_millis(); // calculate by msvc
		frametime = tmp;
	} while (1);

	Draw_Text(phost, "Done Callist Record");
	EVE_sleep(2000);
}

#endif // EVE_EXT_CALLLIST_ENABLE

/* Nothing beyond this */
