/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
 * RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
 * RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 *
 * Abstract: Application to demonstrate function of EVE.
 * Author : Bridgetek
 *
 * Revision History:
 * 1.0 - date 2018.07.30 - Initial for BT816
 */

#include "Common.h"
#include "EVE_CoCmd.h"
#include "EV_ChargePoint_Transport.h"
#include "App_BarLCD_EW2020.h"
#include "App.h"
#include "Uart.h"

#include "Page_Init_Logo.h"
#include "Page_Init_Textdown.h"
#include "Page_Init_CarBlow.h"

static void Test_App_BarLCD_EW2020();

static E_PAGE mLastPage = 0;
static AP current;

const uint32_t img_bgshort             = 200;
const uint32_t img_carleft             = 201;
const uint32_t img_fullbg              = 202;
const uint32_t img_light               = 203;
const uint32_t img_pauthen             = 204;
const uint32_t img_pcharge             = 205;
const uint32_t img_planguage           = 206;
const uint32_t img_pplug               = 207;
const uint32_t img_preport             = 208;
const uint32_t img_pstandby            = 209;
const uint32_t QrCode                  = 210;
const uint32_t authenticate_blue_card  = 211;
const uint32_t authenticate_cell_1     = 212;
const uint32_t authenticate_cell_2     = 213;
const uint32_t authenticate_cell_3     = 214;
const uint32_t authenticate_cell_4     = 215;
const uint32_t authenticate_start      = 216;
const uint32_t Car                     = 217;
const uint32_t language_flag_all       = 218;
const uint32_t language_pointer        = 219;
const uint32_t Logo                    = 220;
const uint32_t pin_cell                = 221;
const uint32_t pin_empty               = 222;
const uint32_t pin_line                = 223;
const uint32_t pin_line_run            = 224;
const uint32_t pin_source              = 225;
const uint32_t plugin_left             = 226;
const uint32_t plugin_right            = 227;

static int Draw_BG() {
	Img_t *bg_car   = &appImages[img_carleft];
	Img_t *bg_empty = &appImages[img_bgshort];
	
	static int isFirstRun = 1;
	if (isFirstRun) {
		isFirstRun = 0;
	}

	Image_Draw(phost, bg_car);
	
	uint32_t w = bg_car->w;
	while (w < ( DispWidth )) {
		bg_empty->x = w;
		Image_Draw(phost, bg_empty);
		w += bg_empty->w - 15;
	}

}
static int Draw_Indicate(uint8_t number) {
	Img_t *indicate = &appImages[number];

	Image_Draw(phost, indicate);
}

static int savePointerX = 0;
static int PL_Init() { 
	static int initX = 0;
	Img_t *authIm[] = { &appImages[language_flag_all], &appImages[language_pointer]};

	if (initX == 0) {
		for (int i = 0; i < 2; i++) {
			authIm[i]->x += DispWidth / 2 - 150;
			authIm[i]->y += DispHeight / 2 - authIm[0]->h / 2;
		}
		initX = 1;
		savePointerX = appImages[language_pointer].x;
	}

	return 0;
}
static int PL_Draw(E_PAGE param) {
	Display_Start(phost);
	
	Draw_BG();
	Draw_Indicate(img_planguage);

	Img_t *authIm[] = { &appImages[language_flag_all], &appImages[language_pointer]};
	static int loop = 0;
	static int step = 1;
	static int space = 0;

	appImages[language_pointer].x = savePointerX + space;

	for (int i = 0; i < 2; i++) {
		Image_Draw(phost, authIm[i]);
	}

	loop++;
	if (loop > 5) {
		space += step;
		if (space == 10) {
			step = -1;
		}
		else if (space == 0) {
			step = 1;
		}
		loop = 0;
	}

	Display_End(phost);
	return 0; 
}
static int PA_Init() {
	static int initX = 0;
	Img_t *authIm[] = { 
		&appImages[authenticate_blue_card], 
		&appImages[authenticate_cell_1], 
		&appImages[authenticate_cell_2], 
		&appImages[authenticate_cell_3], 
		&appImages[authenticate_cell_4], 
		&appImages[authenticate_start] };

	if (initX == 0) {
		for (int i = 0; i < 6; i++) {
			authIm[i]->y += 20;
			authIm[i]->x += DispWidth / 2 - 100;
		}
		initX = 1;
	}
	
	return 0; 
}
static int PA_Draw(E_PAGE param) {
	Display_Start(phost);
	Draw_BG();
	Draw_Indicate(img_pauthen);

	Img_t *authIm[] = {
		0,
		&appImages[authenticate_cell_1],
		&appImages[authenticate_cell_2],
		&appImages[authenticate_cell_3],
		&appImages[authenticate_cell_4],
	}; 
	
	static int power = 1;
	static int card = 0;
	static int loop = 0;

	Image_Draw(phost, &appImages[authenticate_start]);
	if (power > 0) {
		Image_Draw(phost, authIm[power]);
	}
	if (card) {
		Image_Draw(phost, &appImages[authenticate_blue_card]);
	}

	loop++;
	if (loop > 10) {
		power++;
		card++;

		if (power == 5) {
			power = 0;
		}
		if (card == 2) {
			card = 0;
		}
		loop = 0;
	}
	Display_End(phost);
	return 0;
}
static int saveX1PP;
static int saveX2PP;
static int PP_Init() {
	static int initX = 0;
	Img_t *authIm[] = { &appImages[plugin_left], &appImages[plugin_right] };
	if (initX == 0) {
		for (int i = 0; i < 2; i++) {
			authIm[i]->x += DispWidth / 2 - 100;
			authIm[i]->y = DispHeight / 2 - authIm[i]->h / 2;
		}
		initX = 1;
		saveX1PP = authIm[0]->x;
		saveX2PP = authIm[1]->x;
	}
	
	return 0; 

}
static int PP_Draw(E_PAGE param) {
	Display_Start(phost);
	Draw_BG();
	Draw_Indicate(img_pplug);
	
	Img_t *authIm[] = { &appImages[plugin_left], &appImages[plugin_right] };
	static int space = 0;
	static int step = 1;
	static int loop = 0;

	authIm[0]->x = saveX1PP - space;
	authIm[1]->x = saveX2PP + space;

	Image_Draw(phost, authIm[0]);
	Image_Draw(phost, authIm[1]);

	loop++;
	if (loop > 2) {
		space += step;
		if (space > 5) {
			step = -1;
		}
		else if (space == 0) {
			step = 1;
		}

		loop = 0;
	}
	Display_End(phost);
	return 0;
}
static int saveX1PC;
static int saveX2PC;
static int PC_Init() { 
	static int initX = 0;
	Img_t *authIm[] = { &appImages[pin_cell], &appImages[pin_empty], &appImages[pin_line], &appImages[pin_line_run], &appImages[pin_source] };
	if (initX == 0) {
		for (int i = 0; i < 5; i++) {
			authIm[i]->isFlash = 0;
			authIm[i]->x += DispWidth / 2 - 150;
			authIm[i]->y += DispHeight / 2 - appImages[pin_empty].h / 2 + 6;
		}
		initX = 1;
		saveX1PC = appImages[pin_line_run].x;
		saveX2PC = appImages[pin_cell].x;
	}
	
	return 0; 
}
static int PC_Draw(E_PAGE param) {
	Display_Start(phost);
	Draw_BG();
	Draw_Indicate(img_pcharge);
	Img_t *authIm[] = { &appImages[pin_empty], &appImages[pin_line], &appImages[pin_line_run], &appImages[pin_source]};
	#define pinLightLen 130
	static int space = pinLightLen;
	static int step = 1;
	static int loop = 0;

	if (param > 100 || param < 0) {
		param = 0;
	}

	int num = param / 20;

	appImages[pin_line_run].x = saveX1PC + space;

	for (int i = 0; i < 4; i++) {
		Image_Draw(phost, authIm[i]);
	}

	if (num > 0 && num <= 5) {
		for (int i = 0; i < num; i++) {
			appImages[pin_cell].x = saveX2PC + i * 16;
			Image_Draw(phost, &appImages[pin_cell]);
		}
	}

	loop++;
	if (loop > 0) {
		space -= step;
		if (space == 0) {
			space = pinLightLen;
		}

		loop = 0;
	}
	Display_End(phost);
	return 0;
}
static int PR_Init() { return 0; }
static int PR_Draw(E_PAGE param) {
	Display_Start(phost);
	Draw_BG();
	Draw_Indicate(img_preport);
	
	Img_t *qrCode = &appImages[QrCode];

	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

	Gpu_CoCmd_Text(phost, DispWidth / 2 - 100, DispHeight / 2, 29, OPT_CENTERX | OPT_CENTERY, "Please get receipt by scanning QR code ");
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

	qrCode->isFlash = 1;
	qrCode->x = DispWidth / 2 + 120;
	qrCode->y = DispHeight / 2 - qrCode->h / 2;

	Image_Draw(phost, qrCode);
	Display_End(phost);
	return 0;

}

static float x = 1.5;
static float timeFraction = 0.001;
static char strNo = 0;

static int PS_Init() { 
	strNo = 0;
	timeFraction = 0.001;
	return 0; 
}
static int PS_Draw(E_PAGE param) {
	Display_Start(phost);
	Draw_BG();
	Draw_Indicate(img_pstandby);
	
	const char str1[] = " EV Charge Point Demo ";
	//const char str2[] = " Bridgetek Embedded World 2020 ";
	const char str2[] = " EV Charge Point Demo "; // configuration for EW2023
	char s[sizeof(str2)];
	static char lasttam = 0;

	char *str[] = { str1, str2 };
	char len[] = { sizeof(str1), sizeof(str2) };

	const length = 400;
	const float speed = 0.0015;

	//App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0)); /// black text on white bg

	timeFraction += speed;

	float z = Math_Power(timeFraction, 2);
	int t = length * (float)((x + 1) * timeFraction - x) * z;

	if (t < 0) {
		t *= -1;
		lasttam = 1;
	}
	else if (lasttam == 1) {
		strNo = 1;
		lasttam = 0;
	}

	/// End of string
	if (t >= len[strNo]) {
		t = len[strNo] - 1;

		/// End of animation
		if (strNo == 1) {
			timeFraction = 0.001;
			strNo = 0;
			EVE_sleep(1000);
			return 1;
		}
	}

	memset(s, 0, sizeof(s));
	if (t > len[strNo]) {
		t = len[strNo];
	}

	memcpy(s, str[strNo], t);
	Gpu_CoCmd_Text(phost, DispWidth / 2, DispHeight / 2, 30, OPT_CENTERX | OPT_CENTERY, s);

	Display_End(phost);
	return 0;
}

static void Switch_Page(E_PAGE e) {
	if (e == mLastPage) {
		return;
	}
	APP_DBG("Switched to by %d to %s", e, getPagename(e));

	mLastPage = e;
	switch (e) {
	case PAGE_REPORT:
		current.Init = &PR_Init;
		current.Draw = &PR_Draw;
		break;
	case PAGE_SCREEN_SAVER:
		current.Init = &PS_Init;
		current.Draw = &PS_Draw;
		break;
	case PAGE_CHOOSE_LANGUAGE:
		current.Init = &PL_Init;
		current.Draw = &PL_Draw;
		break;
	case PAGE_AUTHENTICATE:
		current.Init = &PA_Init;
		current.Draw = &PA_Draw;
		break;
	case PAGE_PLUGIN:
		current.Init = &PP_Init;
		current.Draw = &PP_Draw;
		break;
	case PAGE_TRANSACTION:
	case PAGE_CHARGING:
		current.Init = &PC_Init;
		current.Draw = &PC_Draw;
		break;
	default:
		APP_ERR("Sync fall");
		return;
	}
}

static int App_Get_New_Signal(){
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	static E_PAGE e;
	UART_RECV_T *recv;

	/// Get signal from Uart
	recv = AppUart_recv();

	/// If new signal come
	if (recv->size > 0) {
		e = recv->data[0];
		if (e >= PAGE_SCREEN_SAVER && e < PAGE_MAX) {
			/// Switch to new page
			Switch_Page(e);
			current.Init();
		}
		else if (e < PAGE_SCREEN_SAVER ) {
			/// Draw percent charging
			current.Draw(e);
		}
		else {
			/// Language selection, ignore
		}
		return 1;/// Got new signal
	}
	else {
		return current.Draw(e);
	}

	return 0;///Nothing received
#endif
	return 0;
}

void App_Server_Move(){
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	char testinput[] = {
			PAGE_CHOOSE_LANGUAGE,
			LANG_DE,
			PAGE_AUTHENTICATE,
			PAGE_PLUGIN,
			PAGE_TRANSACTION,
			PAGE_CHARGING,
			SYNC_CHARGE_PERCENT(10),
			SYNC_CHARGE_PERCENT(20),
			SYNC_CHARGE_PERCENT(30),
			SYNC_CHARGE_PERCENT(40),
			SYNC_CHARGE_PERCENT(50),
			SYNC_CHARGE_PERCENT(60),
			SYNC_CHARGE_PERCENT(70),
			SYNC_CHARGE_PERCENT(80),
			SYNC_CHARGE_PERCENT(90),
			SYNC_CHARGE_PERCENT(100),
			PAGE_REPORT,
			PAGE_SCREEN_SAVER,
	};
	int i = 0;
	while (1) {
		Ev_Send_Page(&testinput[i]);
		i++;

		if(i == sizeof(testinput)){
			i=0;
		}
		EVE_sleep(2000);
	}
#endif
}

void App_BarLCD_EW2020_2nd() {
#if !ENABLE_SERVER_MODE
	FlashHelper_SwitchFullMode(phost);
	for (int i = 0; i < 230; i++) {
		Image_Copy_To_RamG(phost, &appImages[i], 0);
	}
#endif
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	AppUart_setup();
	AppUart_Enable_Uart1();
#endif

#if ENABLE_SERVER_MODE
	App_Server_Move();
#endif

	IMG_PAGES.y = 120 / 2 - IMG_PAGES.h / 2;
	IMG_CAR.y = 120 / 2 - IMG_CAR.h / 2;
	IMG_PAGES.x = -98;
	IMG_CAR.x = 5;

	current.Init= PS_Init;
	current.Draw = PS_Draw;
	current.Init();

	/// Init page at the starting
	PILG_Init();
	while (!PILG_Draw(0)) {}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	if(0)
#endif
	{
		current.Draw = PT_Draw;
		PT_Init();
		while (!App_Get_New_Signal()) {
		}
		EVE_sleep(1000);

//		current.Draw = PICB_Draw;
//		PICB_Init();
//		while (!App_Get_New_Signal()) {
//		}
//		EVE_sleep(1000);
	}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	Test_App_BarLCD_EW2020();
#else
	while (1) {
		App_Get_New_Signal();
	}
#endif
}

static void Test_App_BarLCD_EW2020() {
	if (0) {
		E_PAGE param = 100;
		//PT_Init();
		//PL_Init();
		//PA_Init();
		//PP_Init();
		PC_Init();
		//PR_Init();
		//PS_Init();

		while (1) {
			Display_Start(phost);

			//PT_Draw(param);
			//PL_Draw(param);
			//PA_Draw(param);
			//PP_Draw(param);
			PC_Draw(param);
			//PR_Draw(param);
			//PS_Draw(param);

			Display_End(phost);
			EVE_sleep(10);
		}
	}

	E_PAGE e;

	char testinput[] = {
			PAGE_CHOOSE_LANGUAGE,
			PAGE_AUTHENTICATE,
			PAGE_PLUGIN,
			PAGE_TRANSACTION,
			PAGE_CHARGING,
			PAGE_REPORT,
			PAGE_SCREEN_SAVER,
			PAGE_MAX,
	};

	int i = 0;
	int data = 1;
	while (1) {
		e = testinput[i];
		i++;
		if (i > sizeof(testinput)) {
			i = 0;
		}

		if (e < PAGE_SCREEN_SAVER || e >= PAGE_MAX) {
			data++;
			current.Draw(data * 4 / 3);
			EVE_sleep(10);
			continue;
		}

		EVE_sleep(1000);

		Switch_Page(e);
		current.Init();
		data = 0;

		for (int t = 0; t < 400; t++) {
			data++;
			current.Draw(data * 1 / 5);
			EVE_sleep(10);
		}
	}
}
