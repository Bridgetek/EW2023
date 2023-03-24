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
#include "App.h"

#include "Def.h"
#include "Screen_Saver.h"
#include "Choose_Language.h"
#include "Authenticate.h"
#include "Plugin.h"
#include "Transaction.h"
#include "Charging.h"
#include "Report.h"

static EVE_HalContext ph;
EVE_HalContext *phost;

E_LANG g_language = LANG_EN;
static const E_PAGE PAGE_HOME = PAGE_BEGIN;
static E_PAGE current_page = PAGE_MAX;
static E_PAGE save_current_page = PAGE_BEGIN;

typedef void(*P_FUNC)();

static P_FUNC fp_draw = 0;
static P_FUNC fp_process = 0;

uint32_t SS_START;

P_FUNC ARR_FUNC_INIT[] = { &ss_init, &cl_init, &ath_init, &pl_init, &tr_init, &ch_init, &rp_init };
P_FUNC ARR_FUNC_DRAW[] = { &ss_drawing, &cl_drawing, &ath_drawing, &pl_drawing, &tr_drawing, &ch_drawing, &rp_drawing };
P_FUNC ARR_FUNC_PROCESS[] = { &ss_process_event, &cl_process_event, &ath_process_event, &pl_process_event, &tr_process_event, &ch_process_event, &rp_process_event };

void restart_screen_saver() {
    SS_START = GET_CURR_MILLIS();
}

void switch_page(E_PAGE page) {

    if (page < PAGE_BEGIN && page >= PAGE_MAX)
        return;
    current_page = page;


    Display_End(phost);

    int index = current_page - PAGE_BEGIN;
    fp_draw = ARR_FUNC_DRAW[index];
    fp_process = ARR_FUNC_PROCESS[index];
    ARR_FUNC_INIT[index]();
    restart_screen_saver();

    sync_up((uint8_t)page);
}

void switch_next_page() {
    current_page++;

    if (current_page >= PAGE_MAX) {
        current_page = PAGE_BEGIN;
}
    save_current_page = current_page;
    switch_page(current_page);
}

void switch_page_from_screen_saver() {
    current_page = save_current_page;

    if (current_page == PAGE_BEGIN) {
        current_page++;
    }
    else if (current_page >= PAGE_MAX) {
        current_page = PAGE_BEGIN;
    }
    switch_page(current_page);
}

void processEvents() {
    uint32_t time_lap = GET_CURR_MILLIS() - SS_START;

    Gesture_Renew(phost);

    fp_process();
    if (time_lap > SS_TIME_OUT) {
        SS_START = GET_CURR_MILLIS();
        switch_page(PAGE_SCREEN_SAVER);
    }
}

static void mainLoop() {
	// Store calibration setting
    Gpu_Hal_Wr32(phost, REG_TOUCH_TRANSFORM_A, 0xfffefefc);
    Gpu_Hal_Wr32(phost, REG_TOUCH_TRANSFORM_B, 0xfffffcbf);
    Gpu_Hal_Wr32(phost, REG_TOUCH_TRANSFORM_C, 0x506adb4);
    Gpu_Hal_Wr32(phost, REG_TOUCH_TRANSFORM_D, 0xfffffed1);
    Gpu_Hal_Wr32(phost, REG_TOUCH_TRANSFORM_E, 0xfffefc79);
    Gpu_Hal_Wr32(phost, REG_TOUCH_TRANSFORM_F, 0x32c3211);

    FlashHelper_SwitchFullMode(phost);
    Gpu_CoCmd_FlashRead(phost, RAM_G, 4096, RAM_DL_SIZE);
    App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(FRACTION));
    switch_language();
    switch_page(PAGE_HOME);
    SS_START = GET_CURR_MILLIS();

    EVE_Util_clearScreen(phost);
	while (TRUE) {
        Display_Start(phost);
        App_WrCoCmd_Buffer(phost, VERTEX_FORMAT(FRACTION));

        fp_draw();
        processEvents();
        Display_End(phost);
    }
}

#if defined(ARDUINO_PLATFORM)
void EVE_emuMain(int argc, char *argv[])
#else
int32_t main(int32_t argc, char8_t *argv[])
#endif
{
	phost = &ph;
	Gpu_Init(phost);

	{
//		Use ExtSync module by REG_PCLK==1 and set REG_PCLK_FREQ to:
//		78 Mhz (0x8D1) for 1280x800 resolution
//		54 MHz (0x891) for 1440x540 resolution
//		12 MHz (0x21) for 1280x120 resolution.
#if defined(DISPLAY_RESOLUTION_WXGA)
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x8D1 );
		EVE_Hal_wr32(phost, REG_PCLK_2X, 1 );
#elif defined(DISPLAY_RESOLUTION_1440x540)
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x891 );
#elif defined(DISPLAY_RESOLUTION_1280x120)
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x21 );
#endif

		APP_DBG("REG_HCYCLE        : %d   ", EVE_Hal_rd16(phost, REG_HCYCLE           ));
		APP_DBG("REG_HOFFSET       : %d   ", EVE_Hal_rd32(phost, REG_HOFFSET          ));
		APP_DBG("REG_HSYNC0        : %d   ", EVE_Hal_rd32(phost, REG_HSYNC0           ));
		APP_DBG("REG_HSYNC1        : %d   ", EVE_Hal_rd32(phost, REG_HSYNC1           ));
		APP_DBG("REG_VCYCLE        : %d   ", EVE_Hal_rd32(phost, REG_VCYCLE           ));
		APP_DBG("REG_VOFFSET       : %d   ", EVE_Hal_rd32(phost, REG_VOFFSET          ));
		APP_DBG("REG_VSYNC0        : %d   ", EVE_Hal_rd32(phost, REG_VSYNC0           ));
		APP_DBG("REG_VSYNC1        : %d   ", EVE_Hal_rd32(phost, REG_VSYNC1           ));
		APP_DBG("REG_SWIZZLE       : %d   ", EVE_Hal_rd32(phost, REG_SWIZZLE          ));
		APP_DBG("REG_PCLK_POL      : %d   ", EVE_Hal_rd32(phost, REG_PCLK_POL         ));
		APP_DBG("REG_HSIZE         : %d   ", EVE_Hal_rd32(phost, REG_HSIZE            ));
		APP_DBG("REG_VSIZE         : %d   ", EVE_Hal_rd32(phost, REG_VSIZE            ));
		APP_DBG("REG_CSPREAD       : %d   ", EVE_Hal_rd32(phost, REG_CSPREAD          ));
		APP_DBG("REG_DITHER        : %d   ", EVE_Hal_rd32(phost, REG_DITHER           ));
		APP_DBG("REG_OUTBITS       : %d   ", EVE_Hal_rd32(phost, REG_OUTBITS          ));
		APP_DBG("REG_PCLK          : %d   ", EVE_Hal_rd32(phost, REG_PCLK             ));
		APP_DBG("REG_PCLK_FREQ     : 0x%3x", EVE_Hal_rd32(phost, REG_PCLK_FREQ        ));
		APP_DBG("REG_FREQUENCY     : %d   ", EVE_Hal_rd32(phost, REG_FREQUENCY        ));

		APP_DBG("System clock: %d (Hz)", Get_SystemClock(phost));
	}

#if 0 && !defined(BT8XXEMU_PLATFORM)
	int sent = Ftf_Write_File_To_Flash_With_Progressbar(phost, TEST_DIR "/Flash/BT81X_Flash_EvChargePoint.bin", "BT81X_Flash_EvChargePoint.bin", 0);
	if(0 == sent){
		Draw_Text(phost,"Error: Cannot open file: " TEST_DIR "/Flash/BT81X_Flash_EvChargePoint.bin");
		while(true){
			EVE_sleep(3000);
		}
	}
#endif
	mainLoop();

#if defined(MSVC_PLATFORM) || defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	return 0;
#endif
}
