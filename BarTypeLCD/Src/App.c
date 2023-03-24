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
#include "App_BarLCD_EW2020.h"
#include "App_BarLCD_EW2020_2nd.h"

static EVE_HalContext ph;
EVE_HalContext *phost;

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
#elif defined(DISPLAY_RESOLUTION_1440x540)
		EVE_Hal_wr32(phost, REG_PCLK_FREQ, 0x891 );
#elif defined(DISPLAY_RESOLUTION_UWB)
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

    Setup_Precision(2);

	Flash_Init(phost, TEST_DIR "/Flash/BT81X_Flash_BarTypeLCD.bin", "BT81X_Flash_BarTypeLCD.bin");

#if UI1_ENABLE
	App_BarLCD_EW2020();
#elif UI2_ENABLE
	App_BarLCD_EW2020_2nd();
#endif

	return 0;
}
