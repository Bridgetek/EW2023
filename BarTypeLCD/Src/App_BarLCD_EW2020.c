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
#include "Uart.h"
#include "App.h"

#include "Page_Authenticate.h"
#include "page_Charging.h"
#include "Page_Chooselanguage.h"
#include "Page_Init_Logo.h"
#include "Page_Plugin.h"
#include "Page_Report.h"

#include "Page_Init_Textdown.h"
#include "Page_Init_CarBlow.h"

void Test_App_BarLCD_EW2020();

E_PAGE mLastPage = 0;
AP current;
int lastAngle = 0;
const int step = -1;

static void Page_End() {
	/// setting REG_PCLK to 0 to avoid display glitch.
	uint32_t addr = 2 * DispWidth * DispHeight;
	Gpu_CoCmd_Snapshot(phost, addr);

	APP_DBG("Snapping Start");
	int y = 0;
	while (y > (int) -DispHeight) {
		Gpu_CoCmd_Snapshot(phost, addr);
		Display_Start(phost);
		EVE_Cmd_waitFlush(phost);
		AL_Img_Draw_From_RAM_G(addr, 0, y, DispWidth, DispHeight, ARGB4, 0);
		y -= 5;
		Display_End(phost);
		APP_DBG("Snapping at %d", y);
	}
	APP_DBG("Snapping Done");
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
		current.DeInit = &PR_DeInit;
		current.Draw = &PR_Draw;
		break;
	case PAGE_SCREEN_SAVER:
		current.Init = &PT_Init;
		current.DeInit = &PT_DeInit;
		current.Draw = &PT_Draw;
		break;
	case PAGE_CHOOSE_LANGUAGE:
		current.Init = &PL_Init;
		current.DeInit = &PL_DeInit;
		current.Draw = &PL_Draw;
		break;
	case PAGE_AUTHENTICATE:
		current.Init = &PA_Init;
		current.DeInit = &PA_DeInit;
		current.Draw = &PA_Draw;
		break;
	case PAGE_PLUGIN:
		current.Init = &PP_Init;
		current.DeInit = &PP_DeInit;
		current.Draw = &PP_Draw;
		break;
	case PAGE_TRANSACTION:
	case PAGE_CHARGING:
		current.Init = &PC_Init;
		current.DeInit = &PC_DeInit;
		current.Draw = &PC_Draw;
		break;
	default:
		APP_ERR("Sync fall");
		return;
	}
}

void App_BarLCD_EW2020() {
	E_PAGE e;
	int i=0;

#if !ENABLE_SERVER_MODE
	FlashHelper_SwitchFullMode(phost);
#endif

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	AppUart_setup();
	AppUart_Enable_Uart1();
#endif

#if ENABLE_SERVER_MODE
	initAppRTC();
	testEV();
#endif

	for (int i = 0; i < 228; i++) {
		Image_Copy_To_RamG(phost, &appImages[i], 0);
	}

	IMG_PAGES.y = 120 / 2 - IMG_PAGES.h / 2;
	IMG_CAR.y = 120 / 2 - IMG_CAR.h / 2;
	IMG_PAGES.x = -98;
	IMG_CAR.x = 5;

	current.Init = &PILG_Init;
	current.DeInit = &PILG_DeInit;
	current.Draw = &PILG_Draw;

	/// Init page at the starting
	PILG_Init();
	while(!PILG_Draw(0)){
		;
	}
	PILG_DeInit();

	{
		PT_Init();
		while (!PT_Draw(0)) {
			;
		}
		PT_DeInit();
		EVE_sleep(1000);

		PICB_Init();
		while (!PICB_Draw(0)) {
			;
		}
		PICB_DeInit();
		EVE_sleep(1000);

	}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	Test_App_BarLCD_EW2020();
#elif defined(FT9XX_PLATFORM) 
	UART_RECV_T *recv;
	while (1) {
		/// Get signal from Uart
		recv = AppUart_recv();

		/// If new signal come
		if(recv->size > 0){
			e = recv->data[0];
			//APP_DBG("Receive: %d", e);
			if (e >= PAGE_SCREEN_SAVER && e < PAGE_MAX){
				/// Switch to new page
				current.DeInit();
				Switch_Page(e);
				current.Init();
			}else if (e < PAGE_BEGIN){
				/// Draw percent charging
				current.Draw(e);
			}else{
				/// Language selection, ignore
			}
		}else{
			current.Draw(e);
		}
	}
#elif defined(RP2040_PLATFORM) 
	UART_RECV_T *recv;
	while (1) {
		/// Get signal from Uart
		recv = AppUart_recv();

		/// If new signal come
		if(recv->size > 0){
			e = recv->data[0];
			//APP_DBG("Receive: %d", e);
			if (e >= PAGE_SCREEN_SAVER && e < PAGE_MAX){
				/// Switch to new page
				current.DeInit();
				Switch_Page(e);
				current.Init();
			}else if (e < PAGE_BEGIN){
				/// Draw percent charging
				current.Draw(e);
			}else{
				/// Language selection, ignore
			}
		}else{
			current.Draw(e);
		}
	}
#endif
}

void Test_App_BarLCD_EW2020() {
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

		current.DeInit();
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
