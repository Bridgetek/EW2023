/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

#include "EVE_Util.h"
#include "EVE_Platform.h"
#include "EVE_HalImpl.h"

EVE_HAL_EXPORT void EVE_CoDlImpl_resetCoState(EVE_HalContext *phost);

static eve_progmem_const uint8_t c_DlCodeBootup[12] = {
	0, 0, 0, 2, // GPU instruction CLEAR_COLOR_RGB
	7, 0, 0, 38, // GPU instruction CLEAR
	0, 0, 0, 0, // GPU instruction DISPLAY
};

/* NOTE: Update `EVE_DISPLAY_T` in `EVE_Util.h` when adding display presets. */
static const uint16_t s_DisplayResolutions[EVE_DISPLAY_NB][4] = {
	/* width, height, refreshRate, hsfWidth */

	{ 480, 272, 60, 0 }, /* EVE_DISPLAY_DEFAULT (values ignored) */

	/* Landscape */
	{ 320, 240, 50, 0 }, /* EVE_DISPLAY_QVGA_320x240_50Hz */
	{ 480, 272, 60, 0 }, /* EVE_DISPLAY_WQVGA_480x272_60Hz */
	{ 800, 480, 60, 0 }, /* EVE_DISPLAY_WVGA_800x480_60Hz */
	{ 1024, 600, 83, 0 }, /* EVE_DISPLAY_WSVGA_1024x600_83Hz */
	{ 1280, 720, 58, 0 }, /* EVE_DISPLAY_HDTV_1280x720_58Hz */
	{ 1280, 800, 65, 0 }, /* EVE_DISPLAY_WXGA_1280x800_65Hz */
	{ 1280, 120, 67, 0 }, /* EVE_DISPLAY_UWB_1280x120_67Hz */
	{ 1440, 540, 60, 0 }, /* EVE_DISPLAY_USP_1440x540_60Hz */

	/* Portrait */
	{ 320, 480, 60, 0 }, /* EVE_DISPLAY_HVGA_320x480_60Hz */

};

#if defined(_WIN32) || defined(EVE_MULTI_GRAPHICS_TARGET)

/* Interactive display selection */
static const char *s_DisplayNames[EVE_DISPLAY_NB] = {
	"<Default>",

	"QVGA 320x240 50Hz",
	"WQVGA 480x272 60Hz",
	"WVGA 800x480 60Hz",
	"WSVGA 1024x600 83Hz",
	"HDTV 1280x720 58Hz",
	"WXGA 1280x800 65Hz",
	"Ultra Wide Bar 1280x120 67Hz",
	"Ultra Stretched Lcd Display Panel 1440x540 60Hz",

	"HVGA 320x480 60Hz",

};

#endif

#if defined(_WIN32)

/* Interactive platform selection */
static const char *s_HostDisplayNames[EVE_HOST_NB] = {
	"<Unknown>",

	"BT8XX Emulator",
	"FT4222",
	"MPSSE",
	"Embedded",
};

#define EVE_SELECT_CHIP_NB 10

/* Interactive emulator chip selection */
static const char *s_SelectChipName[EVE_SELECT_CHIP_NB] = {
	"FT800",
	"FT801",
	"FT810",
	"FT811",
	"FT812",
	"FT813",
	"BT815",
	"BT816",
	"BT817",
	"BT818",
};

static EVE_CHIPID_T s_SelectChipId[EVE_SELECT_CHIP_NB] = {
	EVE_FT800,
	EVE_FT801,
	EVE_FT810,
	EVE_FT811,
	EVE_FT812,
	EVE_FT813,
	EVE_BT815,
	EVE_BT816,
	EVE_BT817,
	EVE_BT818,
};

#endif

/* VERIFY: Can the emulator handle this? */
#if (!defined(BT8XXEMU_PLATFORM) || defined(EVE_MULTI_PLATFORM_TARGET)) \
    && ((EVE_SUPPORT_CHIPID == EVE_FT811) || (EVE_SUPPORT_CHIPID == EVE_FT813) || defined(EVE_MULTI_GRAPHICS_TARGET))
#define TOUCH_DATA_LEN 1172
static eve_progmem_const uint8_t c_TouchDataU8[TOUCH_DATA_LEN] = {
	26, 255, 255, 255, 32, 32, 48, 0, 4, 0, 0, 0, 2, 0, 0, 0, 34,
	255, 255, 255, 0, 176, 48, 0, 120, 218, 237, 84, 255, 107, 92,
	69, 16, 159, 125, 155, 107, 141, 201, 121, 247, 106, 130, 9,
	225, 244, 238, 37, 246, 146, 52, 63, 53, 98, 172, 53, 48, 243,
	8, 36, 166, 182, 63, 136, 216, 246, 7, 205, 219, 75, 122, 119,
	185, 139, 196, 128, 34, 33, 136, 123, 65, 240, 7, 31, 44, 105,
	41, 69, 72, 12, 210, 166, 6, 17, 12, 104, 64, 165, 73, 133, 134,
	98, 80, 74, 43, 148, 22, 20, 133, 40, 20, 34, 233, 15, 82, 250,
	131, 10, 113, 246, 229, 106, 197, 191, 161, 111, 217, 55, 179,
	59, 59, 59, 243, 153, 47, 251, 135, 15, 58, 204, 11, 109, 114,
	89, 149, 84, 169, 242, 172, 2, 109, 10, 73, 53, 167, 92, 158,
	89, 21, 6, 89, 230, 236, 126, 86, 89, 206, 242, 169, 178, 61,
	187, 115, 62, 85, 190, 199, 133, 129, 44, 132, 249, 90, 12, 243,
	124, 67, 49, 169, 76, 81, 22, 195, 60, 211, 113, 40, 133, 249,
	253, 104, 114, 113, 148, 197, 131, 105, 169, 158, 3, 19, 132,
	65, 28, 187, 240, 25, 112, 3, 147, 99, 235, 163, 80, 98, 63, 10,
	123, 181, 208, 73, 213, 29, 24, 246, 226, 121, 112, 199, 146,
	74, 98, 56, 22, 195, 135, 245, 32, 14, 194, 17, 180, 182, 100,
	46, 169, 220, 114, 244, 103, 171, 114, 252, 41, 222, 143, 97,
	150, 53, 64, 167, 202, 110, 240, 86, 186, 45, 16, 90, 14, 191,
	158, 110, 196, 163, 216, 130, 241, 232, 159, 42, 251, 88, 4,
	169, 90, 80, 42, 163, 226, 248, 168, 206, 97, 132, 136, 169, 69,
	113, 20, 27, 245, 203, 40, 131, 12, 54, 87, 37, 143, 253, 43,
	105, 97, 137, 9, 172, 103, 181, 26, 172, 79, 163, 102, 184, 142,
	57, 83, 0, 61, 139, 144, 159, 79, 135, 193, 62, 61, 0, 157, 250,
	5, 20, 186, 67, 91, 255, 133, 238, 140, 104, 167, 30, 192, 197,
	72, 2, 85, 137, 221, 89, 134, 101, 140, 177, 191, 238, 137, 75,
	105, 137, 23, 97, 18, 226, 31, 175, 98, 31, 174, 165, 141, 90,
	193, 21, 88, 194, 117, 60, 207, 243, 115, 248, 202, 254, 171,
	171, 37, 248, 20, 87, 112, 17, 59, 180, 59, 246, 35, 199, 177,
	83, 175, 35, 140, 93, 224, 187, 172, 45, 169, 172, 108, 29, 47,
	192, 59, 186, 70, 159, 1, 208, 155, 136, 40, 71, 190, 195, 101,
	222, 117, 131, 221, 90, 142, 110, 48, 221, 85, 165, 177, 42,
	173, 169, 82, 89, 165, 78, 149, 138, 136, 110, 224, 13, 108,
	200, 0, 132, 1, 50, 103, 114, 55, 176, 145, 254, 51, 132, 208,
	183, 177, 137, 64, 111, 128, 192, 13, 104, 162, 86, 145, 165,
	198, 255, 141, 86, 209, 68, 79, 146, 84, 87, 33, 163, 95, 132,
	39, 120, 182, 242, 76, 115, 78, 123, 40, 142, 7, 200, 227, 21,
	103, 50, 103, 171, 109, 123, 27, 242, 115, 193, 0, 197, 176, 70,
	63, 141, 125, 212, 35, 14, 8, 95, 28, 20, 246, 116, 155, 30,
	132, 152, 118, 3, 40, 29, 162, 62, 60, 158, 9, 131, 8, 197, 120,
	49, 99, 115, 35, 39, 228, 200, 168, 48, 81, 197, 2, 175, 221,
	224, 205, 140, 173, 148, 4, 87, 13, 148, 94, 19, 235, 216, 205,
	62, 212, 179, 14, 199, 221, 234, 21, 223, 207, 112, 254, 163,
	90, 169, 197, 247, 50, 110, 249, 16, 249, 156, 113, 137, 83, 98,
	146, 163, 23, 215, 115, 16, 146, 173, 102, 142, 158, 74, 160,
	205, 181, 28, 217, 67, 93, 156, 121, 40, 156, 202, 76, 130, 169,
	184, 193, 12, 9, 125, 82, 132, 149, 121, 209, 65, 219, 198, 12,
	47, 80, 7, 15, 182, 163, 97, 52, 171, 206, 211, 20, 186, 229,
	195, 148, 42, 15, 137, 45, 116, 244, 29, 188, 138, 105, 177, 76,
	139, 149, 18, 113, 141, 160, 192, 1, 218, 222, 14, 131, 101, 90,
	224, 17, 195, 239, 51, 113, 156, 194, 111, 245, 21, 49, 137,
	171, 100, 61, 238, 21, 166, 50, 67, 95, 83, 63, 154, 202, 71,
	116, 141, 45, 205, 208, 37, 94, 133, 188, 186, 201, 177, 134,
	82, 86, 117, 84, 109, 46, 137, 52, 237, 214, 102, 248, 22, 253,
	82, 41, 137, 91, 148, 37, 29, 97, 147, 195, 86, 126, 89, 156,
	165, 45, 214, 188, 43, 54, 43, 160, 63, 36, 139, 169, 142, 17,
	73, 175, 31, 255, 230, 232, 164, 175, 244, 161, 197, 38, 71,
	118, 121, 70, 109, 226, 43, 252, 14, 100, 249, 142, 135, 180,
	156, 48, 185, 45, 172, 176, 247, 71, 244, 29, 104, 240, 25, 3,
	37, 216, 78, 179, 159, 101, 171, 9, 63, 225, 95, 166, 6, 126,
	71, 24, 29, 156, 165, 199, 253, 126, 108, 119, 88, 51, 146, 216,
	19, 191, 137, 102, 191, 107, 186, 68, 109, 78, 247, 244, 95,
	130, 99, 94, 134, 49, 183, 128, 158, 84, 207, 58, 245, 129, 9,
	220, 145, 78, 16, 218, 85, 102, 226, 94, 102, 142, 121, 247, 51,
	243, 146, 119, 63, 51, 3, 14, 71, 157, 94, 245, 86, 169, 151,
	134, 156, 99, 212, 206, 189, 148, 208, 9, 109, 239, 43, 242,
	125, 57, 135, 249, 19, 111, 120, 245, 182, 79, 121, 218, 62,
	110, 231, 222, 203, 57, 220, 245, 52, 237, 217, 126, 246, 253,
	183, 89, 95, 59, 59, 145, 238, 165, 15, 28, 91, 45, 86, 239,
	140, 215, 231, 199, 56, 34, 59, 239, 154, 239, 159, 244, 38, 57,
	63, 167, 156, 41, 126, 95, 14, 83, 82, 189, 203, 183, 244, 51,
	186, 33, 39, 12, 234, 244, 105, 156, 79, 111, 161, 100, 186,
	232, 37, 25, 187, 195, 253, 252, 137, 35, 245, 2, 243, 54, 207,
	243, 8, 176, 143, 109, 175, 225, 57, 92, 131, 71, 248, 220, 151,
	222, 113, 255, 28, 26, 114, 131, 139, 17, 130, 111, 24, 65, 63,
	235, 157, 198, 90, 188, 238, 213, 251, 63, 120, 130, 59, 151,
	187, 130, 95, 193, 207, 252, 26, 214, 221, 175, 215, 240, 186,
	211, 85, 217, 197, 107, 67, 123, 113, 15, 217, 42, 252, 201,
	155, 245, 133, 94, 131, 122, 255, 87, 207, 80, 143, 215, 238,
	91, 47, 126, 247, 106, 248, 204, 159, 153, 187, 116, 219, 49,
	116, 45, 115, 147, 182, 48, 242, 4, 190, 240, 127, 118, 224,
	193, 247, 224, 3, 89, 247, 15, 148, 99, 211, 103, 26, 255, 255,
	255, 20, 33, 48, 0, 4, 0, 0, 0, 15, 0, 0, 0, 26, 255, 255, 255,
	32, 32, 48, 0, 4, 0, 0, 0, 0, 0, 0, 0
};

/**
 * @brief Download new touch firmware for FT811 and FT813 chip
 * 
 * @param phost  Pointer to Hal context
 */
static inline void uploadTouchFirmware(EVE_HalContext *phost)
{
	/* bug fix pen up section */
	eve_assert_do(EVE_Cmd_wrProgMem(phost, c_TouchDataU8, TOUCH_DATA_LEN));
	eve_assert_do(EVE_Cmd_waitFlush(phost));
}
#else
static inline void uploadTouchFirmware(EVE_HalContext *phost)
{
	/* no-op */
}
#endif

/**
 * @brief Clear the screen
 * 
 * @param phost  Pointer to Hal context
 */
EVE_HAL_EXPORT void EVE_Util_clearScreen(EVE_HalContext *phost)
{
	EVE_Hal_wrProgMem(phost, RAM_DL, c_DlCodeBootup, sizeof(c_DlCodeBootup));
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);
}

EVE_HAL_EXPORT void EVE_Util_bootupDefaults(EVE_HalContext *phost, EVE_BootupParameters *bootup)
{
	int32_t chipId = EVE_CHIPID;
	(void)chipId;

	memset(bootup, 0, sizeof(EVE_BootupParameters));

#if !defined(EVE_USE_INTERNAL_OSC)
	/* Board without external oscillator will not work when ExternalOsc is enabled */
	bootup->ExternalOsc = true;
#endif

#ifdef EVE_SYSTEM_CLOCK
	bootup->SystemClock = EVE_SYSTEM_CLOCK;
#else
	if (chipId >= EVE_FT800 && chipId <= EVE_BT818)
	{
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
		if (chipId >= EVE_BT815)
		{
			bootup->SystemClock = EVE_SYSCLK_72M; /* 72Mhz is default for BT8xx */
		}
		else
#endif
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
		    if (chipId >= EVE_FT810)
		{
			bootup->SystemClock = EVE_SYSCLK_60M; /* 60Mhz is default for FT81x */
		}
		else
#endif
		{
			bootup->SystemClock = EVE_SYSCLK_48M; /* 48Mhz is default for FT80x */
		}
	}
	else
	{
		/* If the chipid is not known yet, this will reconfigure the system clock during `EVE_Util_bootup`.
		Additional default clock overrides need to be implemented there as well. */
		bootup->SystemClock = EVE_SYSCLK_DEFAULT;
	}
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#if defined(EVE_MULTI_PLATFORM_TARGET)
	if (EVE_HOST == EVE_HOST_FT4222)
	{
		bootup->SpiChannels = EVE_SPI_QUAD_CHANNEL;
		bootup->SpiDummyBytes = 2;
	}
#elif defined(ENABLE_SPI_QUAD)
	bootup->SpiChannels = EVE_SPI_QUAD_CHANNEL;
	bootup->SpiDummyBytes = 2;
#elif defined(ENABLE_SPI_DUAL)
	bootup->SpiChannels = EVE_SPI_DUAL_CHANNEL;
	bootup->SpiDummyBytes = 2;
#else
	bootup->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
	bootup->SpiDummyBytes = 1;
#endif
#endif
}

#ifndef EVE_HARDCODED_DISPLAY_TIMINGS
#define EVE_HARDCODED_DISPLAY_TIMINGS 1
#endif

static bool configDefaultsEx(EVE_HalContext *phost, EVE_ConfigParameters *config, uint32_t width, uint32_t height, uint32_t refreshRate, uint32_t hsfWidth, uint32_t freq)
{
	/* Only false if the output resolution will be wrong.
	Not affected by unsupported HSF or refreshRate */
	bool supportedResolution = true;

	uint32_t screenWidth;
	uint32_t pixels;
	uint32_t minCycles;
	uint32_t maxRate;
	uint32_t pclk;

	uint32_t cycles;
	uint32_t vcycle;
	uint32_t hcycle;
	uint32_t hoffset;
	uint32_t voffset;
	uint32_t hsync1;
	uint32_t vsync1;

	memset(config, 0, sizeof(EVE_ConfigParameters));

	/*
	Refresh rate is as follows:
	REG_VCYCLE * REG_HCYCLE * REG_PCLK is the number of clocks each frame takes,
	Divide REG_FREQUENCY by this number to get the frame refresh rate in Hz.
	REG_HCYCLE must be higher than REG_HSIZE, REG_VCYCLE must be higher than REG_VSIZE.

	By default, FT800 is at 48MHz, FT810 is at 60MHz, and BT815 is increased to 72MHz.
	User may configure this differently in `EVE_Util_bootup`.

	Clocks per frame by clock frequency at 60Hz:
	- 84MHz: 1400k
	- 72MHz: 1200k
	- 60MHz: 1000k
	- 48MHz: 800k
	- 36MHz: 600k
	- 24MHz: 400k

	Clocks required per frame by display resolution at 60Hz:
	Max PCLK at frequency: 24  36  48  60  72  84MHz
	- 320x240: 76.8k        5   7  10  13  15  18
	- 480x272: 130.56k      3   4   6   7   9  10
	*/

	/* Trim oversize displays */
	if (EVE_CHIPID >= EVE_FT810)
	{
#ifdef EVE_SUPPORT_HSF
		if (hsfWidth >= width)
		{
			eve_printf_debug("Screen HSF width %i is too large, disabled\n", (int)config->HsfWidth);
			hsfWidth = 0;
			width = hsfWidth;
			supportedResolution = false;
		}
#endif
		if (width > 2047)
		{
			eve_printf_debug("Line buffer width %i is too large for this configuration\n", (int)config->Width);
			width = 2047;
			supportedResolution = false;
		}
		if (height > 2047)
		{
			eve_printf_debug("Screen height %i is too large for this configuration\n", (int)config->Height);
			height = 2047;
			supportedResolution = false;
		}
	}
	else
	{
		if (width > 493)
		{
			/* 494 and up are not reliable */
			eve_printf_debug("Screen width %i is too large for this configuration\n", (int)config->Width);
			width = 493;
			supportedResolution = false;
		}
		if (height > 511)
		{
			eve_printf_debug("Screen height %i is too large for this configuration\n", (int)config->Height);
			height = 511;
			supportedResolution = false;
		}
	}

#ifdef EVE_SUPPORT_HSF
	if (EVE_Hal_supportHsf(phost))
	{
		/* Render width */
		config->Width = (int16_t)width;

		/* Screen width */
		config->HsfWidth = (int16_t)hsfWidth;
	}
	else
#endif
	    if (hsfWidth)
	{
		/* Use the screen width if HSF is not supported */
		config->Width = (int16_t)hsfWidth;
		eve_printf_debug("Display config specifies HSF but not supported on this platform\n");
	}
	else
	{
		config->Width = (int16_t)width;
	}
	config->Height = (int16_t)height;

	screenWidth = hsfWidth ? hsfWidth : width; /* Use screen width for calculation */
	pixels = screenWidth * height;
	minCycles = pixels + (pixels >> 3); /* pixels * 1.125 */
	maxRate = freq / minCycles;
	if (maxRate < refreshRate)
	{
		/* Trim unsupported framerate */
		eve_printf_debug("Frame rate limited to %d\n", (unsigned int)maxRate);
		refreshRate = maxRate;
		pclk = 1;
	}
	else
	{
		pclk = maxRate / refreshRate;
	}
	config->PCLK = (uint8_t)pclk;

	/* Change PCLKPol if the right edge of the display is noisy,
	or if the display is showing jagged colors. Values 0 or 1.
	This toggles the polarity of the PCLK. */
	config->PCLKPol = 1; /* non-default */

	/* Approximate an average good setting */
	cycles = freq / (refreshRate * pclk);
	vcycle = height * cycles / pixels;
	vcycle = (vcycle + ((vcycle + height) >> 1) + height + height) >> 2;
	hcycle = cycles / vcycle;
	hoffset = (hcycle - screenWidth) >> 1;
	voffset = (vcycle - height) >> 1;
	hsync1 = hoffset >> 1;
	vsync1 = voffset >> 1;
	hoffset += hsync1;
	voffset += vsync1;
	config->HCycle = (int16_t)hcycle;
	config->HSync0 = 0;
	config->HSync1 = (int16_t)hsync1;
	config->HOffset = (int16_t)hoffset;
	config->VCycle = (int16_t)vcycle;
	config->VSync0 = 0;
	config->VSync1 = (int16_t)vsync1;
	config->VOffset = (int16_t)voffset;

	/* Verify */
	eve_assert(config->HSync1);
	eve_assert(config->HOffset > config->HSync1);
	eve_assert(config->HCycle > config->Width);
	eve_assert((config->HCycle - config->Width) > config->HOffset);
	eve_assert(config->VSync1);
	eve_assert(config->VOffset > config->VSync1);
	eve_assert(config->VCycle > config->Height);
	eve_assert((config->VCycle - config->Height) > config->VOffset);

	/* Other options */
	/* Toggle CSpread if you see red and blue fringing on black and white edges */
	config->CSpread = 0; /* non-default */
	/* Change this if RGB colors are swapped */
	config->Swizzle = 0;

	if (EVE_CHIPID >= EVE_FT812)
	{
		config->Dither = 0;
		config->OutBitsR = 8;
		config->OutBitsG = 8;
		config->OutBitsB = 8;
	}
	else
	{
		config->Dither = 1;
		config->OutBitsR = 6;
		config->OutBitsG = 6;
		config->OutBitsB = 6;
	}

	return supportedResolution;
}

EVE_HAL_EXPORT bool EVE_Util_configDefaultsEx(EVE_HalContext *phost, EVE_ConfigParameters *config, uint32_t width, uint32_t height, uint32_t refreshRate, uint32_t hsfWidth)
{
	uint32_t freq = EVE_Hal_rd32(phost, REG_FREQUENCY);
	bool res = configDefaultsEx(phost, config, width, height, refreshRate, hsfWidth, freq);
	eve_printf_debug("Display refresh rate set to %f\n", (float)((double)freq / ((double)config->HCycle * (double)config->VCycle * (double)config->PCLK)));
	return res;
}

EVE_HAL_EXPORT void EVE_Util_configDefaults(EVE_HalContext *phost, EVE_ConfigParameters *config, EVE_DISPLAY_T display)
{
	bool supportedResolution;
	uint32_t freq = EVE_Hal_rd32(phost, REG_FREQUENCY);
	uint32_t width;
	uint32_t height;
	uint32_t refreshRate;
	uint32_t hsfWidth;

	/* NOTE: Update `EVE_DISPLAY_T` in `EVE_Util.h` when adding display presets. */
	if (!display)
	{
		/* Default displays if none was explicitly chosen */
#if defined(DISPLAY_RESOLUTION_QVGA)
		display = EVE_DISPLAY_QVGA_320x240_50Hz;
#elif defined(DISPLAY_RESOLUTION_WQVGA)
		display = EVE_DISPLAY_WQVGA_480x272_60Hz;
#elif defined(DISPLAY_RESOLUTION_WVGA)
		display = EVE_DISPLAY_WVGA_800x480_60Hz;
#elif defined(DISPLAY_RESOLUTION_WSVGA)
		display = EVE_DISPLAY_WSVGA_1024x600_83Hz;
#elif defined(DISPLAY_RESOLUTION_HDTV)
		display = EVE_DISPLAY_HDTV_1280x720_58Hz;
#elif defined(DISPLAY_RESOLUTION_WXGA)
		display = EVE_DISPLAY_WXGA_1280x800_65Hz;
#elif defined(DISPLAY_RESOLUTION_UWB)
		display = EVE_DISPLAY_UWB_1280x120_67Hz;
#elif defined(DISPLAY_RESOLUTION_USP)
		display = EVE_DISPLAY_USP_1440x540_60Hz;
#elif defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		display = EVE_DISPLAY_HVGA_320x480_60Hz;
#else
		if (EVE_CHIPID >= EVE_BT817)
			display = EVE_DISPLAY_WXGA_1280x800_65Hz;
		else if (EVE_CHIPID >= EVE_BT815)
			display = EVE_DISPLAY_WVGA_800x480_60Hz;
		else if (EVE_CHIPID >= EVE_FT810)
			display = EVE_DISPLAY_WVGA_800x480_60Hz;
		else if (EVE_CHIPID >= EVE_FT800)
			display = EVE_DISPLAY_WQVGA_480x272_60Hz;
#endif
	}

	width = s_DisplayResolutions[display][0];
	height = s_DisplayResolutions[display][1];
	refreshRate = s_DisplayResolutions[display][2];
	hsfWidth = s_DisplayResolutions[display][3];
	supportedResolution = configDefaultsEx(phost, config, width, height, refreshRate, hsfWidth, freq);

	/* Known values for specific display models */
	if (display == EVE_DISPLAY_QVGA_320x240_50Hz && freq == 48000000)
	{
		/*
		FT800 known values:
		Resolution: 320x240
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 320;
			config->Height = 240;
			config->HCycle = 408;
			config->HOffset = 70;
			config->HSync0 = 0;
			config->HSync1 = 10;
			config->VCycle = 263;
			config->VOffset = 13;
			config->VSync0 = 0;
			config->VSync1 = 2;
			config->PCLK = 8;
		}
#endif
		config->Swizzle = 2;
		config->PCLKPol = 0;
		config->CSpread = 1;
		config->Dither = 1;
	}
	else if (display == EVE_DISPLAY_WQVGA_480x272_60Hz && freq == 48000000)
	{
		/*
		FT800 known values:
		Display: LBL-T050BPH-01
		REG_FREQUENCY: 48MHz
		REG_PCLK: 5
		REG_HCYCLE: 548
		REG_VCYCLE: 292
		Resolution: 480x272
		Refresh rate: 59.99Hz
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 480;
			config->Height = 272;
			config->HCycle = 548;
			config->HOffset = 43;
			config->HSync0 = 0;
			config->HSync1 = 41;
			config->VCycle = 292;
			config->VOffset = 12;
			config->VSync0 = 0;
			config->VSync1 = 10;
			config->PCLK = 5;
		}
#endif
	}
	else if (display == EVE_DISPLAY_WVGA_800x480_60Hz && freq == 72000000)
	{
		/*
		BT81X known values:
		Display: MTF070TN83-V1
		REG_FREQUENCY: 72MHz
		Resolution: 800x480
		Refresh rate: TBD
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 800;
			config->Height = 480;
			config->HCycle = 928;
			config->HOffset = 88;
			config->HSync0 = 0;
			config->HSync1 = 48;
			config->VCycle = 525;
			config->VOffset = 32;
			config->VSync0 = 0;
			config->VSync1 = 3;
			config->PCLK = 2;
		}
#endif
	}
	else if (display == EVE_DISPLAY_HDTV_1280x720_58Hz && freq == 72000000)
	{
		/*
		BT815 known values:
		REG_FREQUENCY: 72MHz
		Resolution: 1280x720
		Refresh rate: 58Hz
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 1280;
			config->Height = 720;
			config->HCycle = 1650;
			config->HOffset = 40 + 220;
			config->HSync0 = 0;
			config->HSync1 = 40;
			config->VCycle = 750;
			config->VOffset = 5 + 20;
			config->VSync0 = 0;
			config->VSync1 = 5;
			config->PCLK = 1;
		}
#endif
		config->CSpread = 0;
		config->Dither = 0;
		config->PCLKPol = 0;
		config->OutBitsR = 0;
		config->OutBitsG = 0;
		config->OutBitsB = 0;
	}
	else if (display == EVE_DISPLAY_WXGA_1280x800_65Hz && freq == 72000000)
	{
		/*
		BT817 known values:
		REG_FREQUENCY: 72MHz
		Resolution: 1280x800
		Refresh rate: 65Hz
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 1280;
			config->Height = 800;
			config->HCycle = 1411;
			config->HOffset = 120;
			config->HSync0 = 0;
			config->HSync1 = 100;
			config->VCycle = 815;
			config->VOffset = 14;
			config->VSync0 = 0;
			config->VSync1 = 10;
			config->PCLK = 1;
			config->PCLKFreq = 0x8B1;
		}
#endif
		config->CSpread = 0;
		config->Dither = 0;
		config->PCLKPol = 0;
		config->OutBitsR = 0;
		config->OutBitsG = 0;
		config->OutBitsB = 0;
	}
	else if (display == EVE_DISPLAY_UWB_1280x120_67Hz && freq == 72000000)
	{
	/*
	BT817 known values:
	REG_FREQUENCY: 72MHz
	Resolution: 1280x120
	Refresh rate: 67Hz
	*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
	if (supportedResolution)
	{
		config->Width = 1280;
		config->Height = 120;
		config->HCycle = 1362;
		config->HOffset = 18;
		config->HSync0 = 0;
		config->HSync1 = 10;
		config->VCycle = 131;
		config->VOffset = 7;
		config->VSync0 = 0;
		config->VSync1 = 4;
		config->PCLK = 6;
	}
#endif
	config->CSpread = 0;
	config->Dither = 1;
	config->PCLKPol = 0;
	config->OutBitsR = 0;
	config->OutBitsG = 0;
	config->OutBitsB = 0;
	}
	else if (display == EVE_DISPLAY_USP_1440x540_60Hz && freq == 72000000)
	{
	/*
	BT817 known values:
	REG_FREQUENCY: 72MHz
	Resolution: 1440x540
	Refresh rate: 60Hz
	*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
	if (supportedResolution)
	{
		config->Width = 1440;
		config->Height = 540;
		config->HCycle = 1280;
		config->HOffset = 138;
		config->HSync0 = 16;
		config->HSync1 = 88;
		config->VCycle = 838;
		config->VOffset = 8;
		config->VSync0 = 0;
		config->VSync1 = 2;
		config->PCLK = 1;
	}
#endif
	config->CSpread = 0;
	config->Dither = 1;
	config->PCLKPol = 0;
	config->OutBitsR = 0;
	config->OutBitsG = 0;
	config->OutBitsB = 0;
	}
	else if (display == EVE_DISPLAY_WSVGA_1024x600_83Hz && freq == 72000000)
	{
		/*
		BT817 known values:
		REG_FREQUENCY: 72MHz
		Resolution: 1024x600
		Refresh rate: 83Hz
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 1024;
			config->Height = 600;
			config->HCycle = 1344;
			config->HOffset = 160;
			config->HSync0 = 0;
			config->HSync1 = 100;
			config->VCycle = 635;
			config->VOffset = 23;
			config->VSync0 = 0;
			config->VSync1 = 10;
			config->PCLK = 1;
			config->PCLKFreq = 0xD12; // 51Mhz
		}
#endif
		config->CSpread = 0;
		config->Dither = 1;
		config->PCLKPol = 1;
		config->OutBitsR = 0;
		config->OutBitsG = 0;
		config->OutBitsB = 0;
	}

	eve_printf_debug("Display refresh rate set to %f\n", (float)((double)freq / ((double)config->HCycle * (double)config->VCycle * (double)config->PCLK)));
}

#define EXTRACT_CHIPID(romChipId) ((((romChipId) >> 8) & 0xFF) | (((romChipId) & (0xFF)) << 8))

EVE_HAL_EXPORT bool EVE_Util_bootup(EVE_HalContext *phost, EVE_BootupParameters *bootup)
{
	/* IMPORTANT: Do not use EVE_CoCmd functions here, as they can be overridden by hooks */

	const uint32_t expectedChipId = EVE_CHIPID;
	uint8_t engineStatus;
	uint32_t chipId;
	uint8_t id;

	do
	{
		int tries = 0;

		/* EVE will be in SPI Single channel after POR */
		if (!EVE_Hal_powerCycle(phost, true))
			return false;

		if (bootup->ExternalOsc)
		{
			/* Set the clk to external clock. Must disable it when no external clock source on the board */
			EVE_Hal_hostCommand(phost, EVE_EXTERNAL_OSC);
			EVE_sleep(10);
		}
		else
		{
			/* Send CLKINT explicitly to play safe for all Eve series chip*/
			EVE_Hal_hostCommand(phost, EVE_INTERNAL_OSC);
			EVE_sleep(10);
		}

		/* Update system clock as per user selected */
		if (bootup->SystemClock)
		{
			EVE_Host_selectSysClk(phost, bootup->SystemClock);
		}

		/* Access address 0 to wake up the FT800 */
		EVE_Hal_hostCommand(phost, EVE_ACTIVE_M);
		EVE_sleep(300);

		EVE_Host_coreReset(phost);
		/* Wait for valid chip ID */
		chipId = EVE_Hal_rd32(phost, ROM_CHIPID);
		while (EXTRACT_CHIPID(chipId) < EVE_FT800
		    || EXTRACT_CHIPID(chipId) > EVE_BT818)
		{
			eve_printf_debug("EVE ROM_CHIPID after wake up %lx\n", chipId);

			++tries;
			EVE_sleep(20);
			if (phost->CbCmdWait && !phost->CbCmdWait(phost))
				return false;

			chipId = EVE_Hal_rd32(phost, ROM_CHIPID);

#ifdef EVE_MULTI_TARGET
			/* Turn off external clock if chipId reads incorrectly for 4 times */
			/* When not targeting multiple boards, set the EVE_USE_INTERNAL_OSC preprocessor variable, or configure the ExternalOsc parameter at runtime */
			if (tries >= 4)
			{
				bootup->ExternalOsc = !bootup->ExternalOsc;
				eve_printf_debug("Reattempting bootup with ExternalOsc changed from %s to %s\n", bootup->ExternalOsc ? "OFF" : "ON", bootup->ExternalOsc ? "ON" : "OFF");

				EVE_Host_powerModeSwitch(phost, EVE_SLEEP_M); /* Put EVE into Sleep mode, prepare for clock selection */
				chipId = 0; //In case chipId is not zero
				break;
			}
#endif
		}
	} while (!chipId);

	/* Validate chip ID to ensure the correct HAL is used */
	/* ROM_CHIPID is valid across all EVE devices */
	if (expectedChipId && EXTRACT_CHIPID(chipId) != expectedChipId)
		eve_printf_debug("Mismatching EVE chip id %lx, expect model %lx\n", ((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8), expectedChipId);
	eve_printf_debug("EVE chip id %lx %lx.%lx\n", EXTRACT_CHIPID(chipId), ((chipId >> 16) & 0xFF), ((chipId >> 24) & 0xFF));

	/* Switch to the proper chip ID if applicable */
#ifdef EVE_MULTI_GRAPHICS_TARGET
	phost->ChipId = EXTRACT_CHIPID(chipId);
	if (phost->ChipId >= EVE_BT815)
		phost->GpuDefs = &EVE_GpuDefs_BT81X;
	else if (phost->ChipId >= EVE_FT810)
		phost->GpuDefs = &EVE_GpuDefs_FT81X;
	else if (phost->ChipId >= EVE_FT800)
		phost->GpuDefs = &EVE_GpuDefs_FT80X;
#endif

	/* Turn off previous audio playback (in case powerdown is not connected) */
	EVE_Hal_wr32(phost, REG_PLAYBACK_LENGTH, 0);

#ifdef EVE_MULTI_GRAPHICS_TARGET
	/* The default clock configuration is only set when the graphics target is known at compile time */
	if (!bootup->SystemClock)
	{
		if (phost->ChipId >= EVE_BT815)
		{
			/* No clock was set, but we want 72MHz default for BT81X and up */
			eve_printf_debug("Set system clock PLL to 72MHz\n");
			bootup->SystemClock = EVE_SYSCLK_72M;

			/* Sleep */
			EVE_Hal_hostCommand(phost, EVE_SLEEP_M);
			EVE_sleep(300);

			/* Update system clock */
			EVE_Host_selectSysClk(phost, bootup->SystemClock);

			/* Access address 0 to wake up the FT800 */
			EVE_Hal_hostCommand(phost, EVE_ACTIVE_M);
			EVE_sleep(300);
		}
		else if (phost->ChipId >= EVE_FT810)
		{
			/* Assume the default */
			bootup->SystemClock = EVE_SYSCLK_60M;
		}
	}
#endif

	/* Read Register ID to check if EVE is ready. */
	while ((id = EVE_Hal_rd8(phost, REG_ID)) != 0x7C)
	{
		eve_printf_debug("EVE register ID after wake up %x\n", (unsigned int)id);

		EVE_sleep(20);
		if (phost->CbCmdWait && !phost->CbCmdWait(phost))
			return false;
	}
	eve_printf_debug("EVE register ID after wake up %x\n", (unsigned int)id);
	eve_assert(chipId == EVE_Hal_rd32(phost, ROM_CHIPID));

	/* Update REG_FREQUENCY as per user selected */
	if (bootup->SystemClock != EVE_SYSCLK_DEFAULT)
	{
		uint32_t clockMHz = bootup->SystemClock * 12;
		EVE_Hal_wr32(phost, REG_FREQUENCY, clockMHz * 1000 * 1000);
		EVE_Hal_flush(phost);
		eve_printf_debug("EVE clock frequency set to %d MHz\n", (unsigned int)clockMHz);
	}
	else
	{
		eve_printf_debug("EVE default clock is %d MHz\n", (unsigned int)(EVE_Hal_rd32(phost, REG_FREQUENCY) / 1000000));
	}

	/* Switch to configured default SPI channel mode */
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		switch (bootup->SpiChannels)
		{
		case EVE_SPI_QUAD_CHANNEL:
			eve_printf_debug("Quad channel SPI\n");
			break;
		case EVE_SPI_DUAL_CHANNEL:
			eve_printf_debug("Dual channel SPI\n");
			break;
		case EVE_SPI_SINGLE_CHANNEL:
			eve_printf_debug("Single channel SPI\n");
			break;
		default:
			break;
		}
		EVE_Hal_setSPI(phost, bootup->SpiChannels, bootup->SpiDummyBytes);
	}
#endif

	/* In some cases, for example, QSPI on MM817EV board with MM930LITE, 
	   the drive strength is required to adjust for better signal integrity. 
	   Setting to max current 20mA here is to play safe.  Users may adjust the 
	   corresponding bits of REG_GPIOX according to their own situation. */
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_Hal_wr16(phost, REG_GPIOX_DIR, 0xffff);
		EVE_Hal_wr16(phost, REG_GPIOX, 0xffff);
	}
	else
	{
		EVE_Hal_wr8(phost, REG_GPIO_DIR, 0xff);
		EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	}

	/* Update touch firmware */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 6285)
#endif
	if ((EVE_CHIPID == EVE_FT811 || EVE_CHIPID == EVE_FT813) && (EVE_HOST != EVE_HOST_BT8XXEMU))
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	{
#if defined(PANL70) || defined(PANL70PLUS)
		EVE_Hal_wr8(phost, REG_CPURESET, 2);
		EVE_Hal_wr16(phost, REG_TOUCH_CONFIG, 0x05d0);
#endif
		/* Download new firmware to fix pen up issue */
		/* It may cause resistive touch not working any more*/
		uploadTouchFirmware(phost);
#if defined(PANL70) || defined(PANL70PLUS)
		EVE_UtilImpl_bootupDisplayGpio(phost);
#endif
		EVE_Hal_flush(phost);
		EVE_sleep(100);
	}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 6285)
#endif
	if ((EVE_CHIPID == EVE_BT815 || EVE_CHIPID == EVE_BT817) && (EVE_HOST != EVE_HOST_BT8XXEMU))
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	{
#if defined(DISPLAY_RESOLUTION_WXGA)
		//Assume WXGA is using Goodix touch IC
		//To be fixed: use macro for touch configuration
		eve_printf_debug("Setup touch for Goodix\n");
		EVE_Hal_wr8(phost, REG_CPURESET, 2);
		EVE_Hal_wr16(phost, REG_TOUCH_CONFIG, 0x05d0);
		EVE_Hal_wr8(phost, REG_CPURESET, 0);
#endif
	}

	/* Read REG_CPURESET to check if engines are ready.
	Bit 0 for coprocessor engine,
	Bit 1 for touch engine,
	Bit 2 for audio engine.
	*/
	while ((engineStatus = EVE_Hal_rd8(phost, REG_CPURESET)) != 0x00)
	{
		if (engineStatus & 0x01)
		{
			eve_printf_debug("Coprocessor engine is not ready\n");
		}
		if (engineStatus & 0x02)
		{
			eve_printf_debug("Touch engine is not ready\n");
			if (EVE_HOST == EVE_HOST_BT8XXEMU) // FIXME: Emulator REG_CPURESET bits behaviour on FT800 and FT801 doesn't match
			{
				EVE_Hal_wr8(phost, REG_CPURESET, 0);
			}
		}
		if (engineStatus & 0x04)
		{
			eve_printf_debug("Audio engine is not ready\n");
		}

		EVE_sleep(20);
		if (phost->CbCmdWait && !phost->CbCmdWait(phost))
			return false;
	}
	eve_printf_debug("All engines are ready\n");

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
#ifdef EVE_TOUCH_GOODIX
	eve_printf_debug("Goodix configuration\n");
	EVE_Hal_wr8(phost, REG_CPURESET, 2);
	EVE_Hal_wr16(phost, REG_TOUCH_CONFIG, 0x05d0);
	EVE_Hal_wr8(phost, REG_CPURESET, 0);
#endif
#endif

	return true;
}

EVE_HAL_EXPORT bool EVE_Util_config(EVE_HalContext *phost, EVE_ConfigParameters *config)
{
	/* IMPORTANT: Do not use EVE_CoCmd functions here, as they can be overridden by hooks */

	const bool swapXY = EVE_CHIPID >= EVE_FT810 ? EVE_Hal_rd8(phost, REG_ROTATE) & 0x2 : false;
	uint16_t wp;
	uint16_t rp;

	if (EVE_CHIPID < EVE_FT810)
	{
		eve_assert(config->Width < 512);
		eve_assert(config->Height < 512);
	}
	else
	{
		eve_assert(config->Width < 2048);
		eve_assert(config->Height < 2048);
	}

	/* Turn off display output clock */
	EVE_Hal_wr8(phost, REG_PCLK, 0);
	phost->PCLK = 0;

	EVE_Hal_wr16(phost, REG_HCYCLE, config->HCycle);
	EVE_Hal_wr16(phost, REG_HOFFSET, config->HOffset);
	EVE_Hal_wr16(phost, REG_HSYNC0, config->HSync0);
	EVE_Hal_wr16(phost, REG_HSYNC1, config->HSync1);
	EVE_Hal_wr16(phost, REG_VCYCLE, config->VCycle);
	EVE_Hal_wr16(phost, REG_VOFFSET, config->VOffset);
	EVE_Hal_wr16(phost, REG_VSYNC0, config->VSync0);
	EVE_Hal_wr16(phost, REG_VSYNC1, config->VSync1);
	EVE_Hal_wr8(phost, REG_SWIZZLE, config->Swizzle);
	EVE_Hal_wr8(phost, REG_PCLK_POL, config->PCLKPol);
	EVE_Hal_wr16(phost, REG_HSIZE, config->Width);
	EVE_Hal_wr16(phost, REG_VSIZE, config->Height);
	EVE_Hal_wr16(phost, REG_CSPREAD, config->CSpread);
	EVE_Hal_wr16(phost, REG_DITHER, config->Dither);
	EVE_Hal_wr16(phost, REG_OUTBITS,
	    (uint16_t)(((config->OutBitsR & 0x7) << 6)
	        | ((config->OutBitsG & 0x7) << 3)
	        | (config->OutBitsB & 0x7)));
	if (swapXY)
	{
		phost->Width = config->Height;
		phost->Height = config->Width;
	}
	else
	{
		phost->Width = config->Width;
		phost->Height = config->Height;
	}

#if (EVE_SUPPORT_CHIPID >= EVE_BT817) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (EVE_CHIPID >= EVE_BT817 && config->PCLKFreq)
	{
		EVE_Hal_wr16(phost, REG_PCLK_FREQ, config->PCLKFreq); // NOTE: If this should use CMD_PCLKFREQ instead, move the PCLKFreq configuration code to just before the HSF configuration below
		uint32_t bit_8_4 = (config->PCLKFreq & 0b111110000) >> 4; // TODO: Split up PCLKFreq in EVE_ConfigParameters, it may be more user friendly
		uint32_t bit_3_0 = (config->PCLKFreq & 0b111);

		uint32_t pclkFrequency = bit_8_4 * (12 * 1000 * 1000) / bit_3_0 / 2;

		// Send two pixels per system clock to the EXTSYNC block if PCLK_frequency > SystemClock
		if (pclkFrequency > EVE_Hal_rd32(phost, REG_FREQUENCY))
		{
			EVE_Hal_wr32(phost, REG_PCLK_2X, 1);
		}
	}
#endif

	/* 
	TODO:
	EVE_Hal_wr16(phost, REG_ADAPTIVE_FRAMERATE, 1);
	*/

#ifdef RESISTANCE_THRESHOLD /* TODO: From config */
	if (EVE_Hal_isScreenResistive(phost))
	{
		/* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
		/* TODO: From config */
		EVE_Hal_wr16(phost, REG_TOUCH_RZTHRESH, RESISTANCE_THRESHOLD);
	}
#endif

	EVE_Util_clearScreen(phost);

	/* Refresh fifo */
	wp = EVE_Cmd_wp(phost);
	rp = EVE_Cmd_rp(phost);
	EVE_Cmd_space(phost);
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		phost->MediaFifoAddress = 0;
		phost->MediaFifoSize = 0;
	}
#endif

	/* Coprocessor needs a reset */
	if (wp || rp)
	{
		/* If this occurs after powerdown and bootup, powerdown is not working */
		eve_printf_debug("Coprocessor fifo not empty\n");
		if (!EVE_Util_resetCoprocessor(phost))
			return false;
	}
	else
	{
		/* Setup coprocessor defaults */
		eve_printf_debug("Configure coprocessor defaults\n");
		EVE_Cmd_wr32(phost, CMD_DLSTART);
		EVE_Cmd_wr32(phost, CMD_COLDSTART);

		/* Set the latest API level.
		CMD_APILEVEL applicable since BT817. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT817)
		if (EVE_CHIPID >= EVE_BT817)
		{
			EVE_Cmd_wr32(phost, CMD_APILEVEL);
			EVE_Cmd_wr32(phost, 2);
		}
#endif

#ifdef EVE_FLASH_AVAILABLE
		if (EVE_CHIPID >= EVE_BT815)
		{
			/* Reattach flash to avoid inconsistent state */
			EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
		}
#endif

		EVE_CoDlImpl_resetCoState(phost);
	}

#ifdef EVE_SUPPORT_HSF
	if (config->HsfWidth)
	{
		if (EVE_Hal_supportHsf(phost))
		{
			EVE_Cmd_startFunc(phost);
			EVE_Cmd_wr32(phost, CMD_HSF);
			EVE_Cmd_wr32(phost, config->HsfWidth);
			EVE_Cmd_endFunc(phost);
		}
		else
		{
			/* Set width to screen width if HSF is not supported here */
			eve_printf_debug("HSF specified but not supported on this device\n");
			EVE_Hal_wr16(phost, REG_HSIZE, config->HsfWidth);
			if (swapXY)
				phost->Height = config->HsfWidth;
			else
				phost->Width = config->HsfWidth;
		}
	}
#endif

	EVE_Hal_flush(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;

	EVE_Hal_wr8(phost, REG_PCLK, config->PCLK); /* After this display is visible on the LCD */
	phost->PCLK = config->PCLK;

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* to cross check reset pin */
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	EVE_Hal_flush(phost);
	EVE_sleep(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0x7f);
	EVE_Hal_flush(phost);
	EVE_sleep(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	EVE_Hal_flush(phost);
	EVE_sleep(120);
#endif

#if defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT)
#if defined(RP2040_PLATFORM)
	EVE_ILI9488_RP2040_bootup(phost);
	eve_printf_debug("after ILI9488 RP2040 bootup\n");
	EVE_Hal_restoreSPI(phost);
#endif
#if defined(FT9XX_PLATFORM)
	EVE_ILI9488_bootup();
	eve_printf_debug("after ILI9488 bootup\n");
	EVE_Hal_restoreSPI(phost);
#endif
#endif

	eve_printf_debug("EVE configuration ready\n");
	return true;
}

EVE_HAL_EXPORT void EVE_Util_shutdown(EVE_HalContext *phost)
{
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_Hal_wr16(phost, REG_GPIOX_DIR, 0xffff);
		EVE_Hal_wr16(phost, REG_GPIOX, 0);
	}
	else
	{
		EVE_Hal_wr8(phost, REG_GPIO_DIR, 0xff);
		EVE_Hal_wr8(phost, REG_GPIO, 0);
	}
	EVE_Hal_wr8(phost, REG_PCLK, 0);
	EVE_Hal_flush(phost);
	EVE_Hal_powerCycle(phost, false);
}

/*
Patch: OTP needs to be reactivated when the coprocessor is reset during CMD_LOGO
Applicable to: FT81X-series
*/
#define EVE_SUBPATCH_PTR 0x7ffeU /* NOTE: This address is only valid for FT81X */
static inline bool EVE_Util_needsSubPatch(EVE_HalContext *phost)
{
	return (EVE_CHIPID >= EVE_FT810) && (EVE_CHIPID <= EVE_FT813);
}

/* 
Patch: Video patch from OTP needs to be reapplied after coprocessor reset
Applicable to: BT81X-series
*/
#define EVE_VIDEOPATCH_ADDR 0x309162UL /* NOTE: This address is only valid for BT815 and BT816 */
static inline bool EVE_Util_needsVideoPatch(EVE_HalContext *phost)
{
	return (EVE_CHIPID >= EVE_BT815) && (EVE_CHIPID <= EVE_BT816);
}

#if defined(_DEBUG)
/* Backup the last 128 bytes of RAM_G, which may be used for an error message */
void debugBackupRamG(EVE_HalContext *phost)
{
	if (!phost->DebugMessageVisible)
	{
		EVE_Hal_rdMem(phost, phost->DebugBackup, RAM_G + RAM_G_SIZE - sizeof(phost->DebugBackup), sizeof(phost->DebugBackup));
		phost->DebugMessageVisible = true;
	}
}
static void debugRestoreRamG(EVE_HalContext *phost)
{
	if (phost->DebugMessageVisible)
	{
		EVE_Hal_wrMem(phost, RAM_G + RAM_G_SIZE - sizeof(phost->DebugBackup), phost->DebugBackup, sizeof(phost->DebugBackup));
		phost->DebugMessageVisible = false;
	}
}
#endif

/**
 * @brief Reset Coprocessor
 * 
 * @param phost  Pointer to Hal context
 * @return true True if successful
 * @return false False if error
 */
EVE_HAL_EXPORT bool EVE_Util_resetCoprocessor(EVE_HalContext *phost)
{
	/* IMPORTANT: Do not use EVE_CoCmd functions here, as they can be overridden by hooks */

	const bool needsVideoPatch = EVE_Util_needsVideoPatch(phost);
	uint16_t videoPatchVector;
	bool ready;
	uint16_t rd;
	uint16_t wr;
	(void)rd;
	(void)wr;

	eve_printf_debug("Reset coprocessor\n");

	if (needsVideoPatch)
	{
		/* BT81X video patch */
		videoPatchVector = EVE_Hal_rd16(phost, EVE_VIDEOPATCH_ADDR);
	}

	/* Set REG_CPURESET to 1, to hold the coprocessor in the reset condition */
	EVE_Hal_wr8(phost, REG_CPURESET, 1);
	EVE_Hal_flush(phost);
	EVE_sleep(100);

	/* Set REG_CMD_READ and REG_CMD_WRITE to zero */
	EVE_Hal_wr16(phost, REG_CMD_READ, 0);
	EVE_Hal_wr16(phost, REG_CMD_WRITE, 0);
	EVE_Hal_wr16(phost, REG_CMD_DL, 0);
	EVE_Hal_wr8(phost, REG_PCLK, phost->PCLK); /* j1 will set the pclk to 0 for that error case */

	/* Stop playing audio in case video with audio was playing during reset */
	EVE_Hal_wr8(phost, REG_PLAYBACK_PLAY, 0);

	if (EVE_Util_needsSubPatch(phost))
	{
		/* Enable patched rom in case the reset is requested while CMD_LOGO is running.
		This is necessary as CMD_LOGO may swap to the other rom page */
		EVE_Hal_wr8(phost, REG_ROMSUB_SEL, 3);
	}

	/* Default */
	phost->CmdFault = false;

	/* Set REG_CPURESET to 0, to restart the coprocessor */
	EVE_Hal_wr8(phost, REG_CPURESET, 0);
	EVE_Hal_flush(phost);
	EVE_sleep(100);
	eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 0);
	eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);

	/* Refresh fifo */
	EVE_Cmd_waitFlush(phost);
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		phost->MediaFifoAddress = 0;
		phost->MediaFifoSize = 0;
	}
#endif

	if (EVE_Util_needsSubPatch(phost))
	{
		/* Clear cmd with CMD_STOP, exiting CMD_EXECUTE may loop over, depending on OTP */
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_CMD);
		for (int i = 0; i < EVE_CMD_FIFO_SIZE; i += 4)
			EVE_Hal_transfer32(phost, CMD_STOP);
		EVE_Hal_endTransfer(phost);

		/* Go back into the patched coprocessor main loop */
		EVE_Hal_wr8(phost, REG_ROMSUB_SEL, 3);
		EVE_Cmd_startFunc(phost);
		EVE_Cmd_wr32(phost, (4294967047UL));
		EVE_Cmd_wr32(phost, EVE_SUBPATCH_PTR);
		EVE_Cmd_wr32(phost, 0);
		EVE_Cmd_endFunc(phost);
		EVE_Hal_flush(phost);
		eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 12);
		eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);

		/* Difficult to check when CMD_EXECUTE is processed when there's an OTP,
		since the read pointer keeps looping back to 0. */
		EVE_sleep(100);
		eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 12);
		eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);

		/* Need to manually stop previous command from repeating infinitely,
		however, this may cause the coprocessor to overshoot the command fifo,
		hence it's been filled with harmless CMD_STOP commands. */
		EVE_Hal_wr16(phost, REG_CMD_WRITE, 0);
		EVE_Hal_flush(phost);
		EVE_sleep(100);

		/* Refresh fifo */
		EVE_Cmd_waitFlush(phost);
		eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 0);
		eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);
	}

	if (needsVideoPatch)
	{
		/* BT81X video patch */
		EVE_Hal_wr16(phost, EVE_VIDEOPATCH_ADDR, videoPatchVector);
	}

	/* Cold start. Ensure that the coprocessor is ready. */
	EVE_Cmd_wr32(phost, CMD_DLSTART);
	EVE_Cmd_wr32(phost, CMD_COLDSTART);

	/* Set the latest API level.
	CMD_APILEVEL applicable since BT817. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT817)
	if (EVE_CHIPID >= EVE_BT817)
	{
		EVE_Cmd_wr32(phost, CMD_APILEVEL);
		EVE_Cmd_wr32(phost, 2);
	}
#endif

#ifdef EVE_FLASH_AVAILABLE
	if (EVE_CHIPID >= EVE_BT815)
	{
		/* Reattach flash to avoid inconsistent state */
		EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
	}
#endif

	/* Wait for coprocessor to be ready */
	ready = EVE_Cmd_waitFlush(phost);
	if (phost->CbCoprocessorReset) /* Notify */
		phost->CbCoprocessorReset(phost, !ready);

#if defined(_DEBUG)
	debugRestoreRamG(phost);
#endif
	EVE_CoDlImpl_resetCoState(phost);
	return ready;
}

/**
 * @brief Bootup Coprocessor
 * 
 * @param phost  Pointer to Hal context
 * @return true True if successful
 * @return false False if error
 */
EVE_HAL_EXPORT bool EVE_Util_bootupConfig(EVE_HalContext *phost)
{
	EVE_BootupParameters bootup;
	EVE_ConfigParameters config;

	EVE_Util_bootupDefaults(phost, &bootup);
	if (!EVE_Util_bootup(phost, &bootup))
	{
		return false;
	}

	EVE_Util_configDefaults(phost, &config, EVE_DISPLAY_DEFAULT);
	if (!EVE_Util_config(phost, &config))
	{
		EVE_Util_shutdown(phost);
		return false;
	}

	return true;
}

/**********************
** INTERACTIVE SETUP **
**********************/

#if defined(_WIN32)

void EVE_Util_selectDeviceInteractive(EVE_CHIPID_T *chipId, size_t *deviceIdx)
{
	char buf[1024];
	EVE_DeviceInfo info;
	int selectedDeviceIdx;
	size_t deviceCount;
	size_t i;

SELECTDEVICE:
	deviceCount = EVE_Hal_list();
	size_t realDeviceCount = 0;
	for (i = 0; i < deviceCount; ++i)
	{
		EVE_Hal_info(&info, i);
		if (info.Host)
		{
			++realDeviceCount;
			*deviceIdx = i;
		}
	}
	if (realDeviceCount > 1)
	{
		buf[0] = '\0';
		printf("Select a device:\n");
		for (i = 0; i < deviceCount; ++i)
		{
			EVE_Hal_info(&info, i);
			if (info.Host)
				printf("- [%d] %s (%s, %s)\n", (unsigned int)i, info.DisplayName, s_HostDisplayNames[info.Host], info.SerialNumber);
		}
		fgets(buf, sizeof(buf), stdin);
		if (sscanf_s(buf, "%i", &selectedDeviceIdx) != 1)
			goto SELECTDEVICE;
		*deviceIdx = selectedDeviceIdx;
		EVE_Hal_info(&info, *deviceIdx);
		if (!info.Host)
			goto SELECTDEVICE;
		printf("\n");
	}
	else if (realDeviceCount > 0)
	{
		EVE_Hal_info(&info, *deviceIdx);
		printf("%s (%s, %s)\n", info.DisplayName, s_HostDisplayNames[info.Host], info.SerialNumber);
	}
	else
	{
		*deviceIdx = -1;
		*chipId = EVE_SUPPORT_CHIPID;
		return;
	}

#ifdef EVE_MULTI_GRAPHICS_TARGET
SelectChipId:
	buf[0] = '\0';
	if (info.Host == EVE_HOST_BT8XXEMU)
	{
		int selectedChipId;
		printf("Select a chip:\n");
		for (i = 0; i < EVE_SELECT_CHIP_NB; ++i)
		{
			if (s_SelectChipId[i] <= EVE_SUPPORT_CHIPID)
				printf("- [%d] %s\n", (unsigned int)i, s_SelectChipName[i]);
		}
		fgets(buf, sizeof(buf), stdin);
		if (sscanf_s(buf, "%i", &selectedChipId) != 1)
			goto SelectChipId;
		if (selectedChipId >= 0 && selectedChipId < EVE_SELECT_CHIP_NB)
			*chipId = s_SelectChipId[selectedChipId];
		else if (selectedChipId >= EVE_FT800 && selectedChipId <= EVE_BT818)
			*chipId = selectedChipId;
		else
			goto SelectChipId;
		printf("\n");
	}
	else
	{
		*chipId = 0;
	}
#else
	*chipId = EVE_SUPPORT_CHIPID;
#endif
}

#ifdef EVE_MULTI_GRAPHICS_TARGET
EVE_HAL_EXPORT void EVE_Util_selectDisplayInteractive(EVE_DISPLAY_T *display)
{
	char buf[1024];
	int selectedDisplay;
	size_t i;

	printf("\n");
SelectDisplay:
	buf[0] = '\0';
	printf("Select a display (or press ENTER to use the default):\n");
	for (i = 1; i < EVE_DISPLAY_NB; ++i)
	{
		printf("- [%d] %s\n", (unsigned int)i, s_DisplayNames[i]);
	}
	fgets(buf, sizeof(buf), stdin);
	if (buf[0] == '\n' || buf[0] == '\r')
	{
		*display = EVE_DISPLAY_DEFAULT;
	}
	else
	{
		if (sscanf_s(buf, "%i", &selectedDisplay) != 1)
			goto SelectDisplay;
		if (selectedDisplay > 0 && selectedDisplay < EVE_DISPLAY_NB)
			*display = selectedDisplay;
		else
			goto SelectDisplay;
	}
	printf("\n");
}
#endif

#endif

#if defined(_WIN32) && defined(EVE_FLASH_AVAILABLE)
void EVE_Util_selectFlashFileInteractive(eve_tchar_t *flashPath, bool *updateFlash, bool *updateFlashFirmware, const EVE_HalParameters *params, const eve_tchar_t *flashFile)
{
	size_t flashPathSz;
	errno_t ferr;
	FILE *f = NULL;

SELECTFLASH:
	*updateFlash = false;
	*updateFlashFirmware = false;
	if (flashFile)
	{
#if defined(EVE_MULTI_PLATFORM_TARGET) || !defined(BT8XXEMU_PLATFORM)
		uint8_t buffer[4096];
#if defined(EVE_MULTI_PLATFORM_TARGET)
		if (params->Host != EVE_HOST_BT8XXEMU)
#endif
		{
			/* Query user if they want to update the flash file on the device */
			printf("Upload flash image (y/n, or press ENTER to skip):\n");
			buffer[0] = '\0';
			fgets(buffer, sizeof(buffer), stdin);
			/* Fast string to bool, reliably defined for strings starting 
			with 0, 1, t, T, f, F, y, Y, n, N, anything else is undefined. */
			*updateFlash = (buffer[0] == '1' || (buffer[0] & 0xD2) == 0x50);
			printf("\n");
			if (*updateFlash)
			{
				printf("Upload flash firmware (y/n, or press ENTER to skip):\n");
				buffer[0] = '\0';
				fgets(buffer, sizeof(buffer), stdin);
				*updateFlashFirmware = (buffer[0] == '1' || (buffer[0] & 0xD2) == 0x50);
				printf("\n");
			}
		}
#endif
		if (*updateFlash
#if defined(EVE_MULTI_PLATFORM_TARGET)
		    || params->Host == EVE_HOST_BT8XXEMU
#elif defined(BT8XXEMU_PLATFORM)
		    || true
#endif
		)
		{
			/* Query user for any changes to the flash file name */
			printf("Select flash file %s(or press ENTER to use %s%ls%s):\n",
			    *updateFlashFirmware ? "with firmware " : "",
			    flashFile[0] ? "\"" : "", /* Quote*/
			    flashFile[0] ? flashFile : L"no flash",
			    flashFile[0] ? "\"" : ""); /* Quote*/

			fgetws(flashPath, MAX_PATH, stdin);
			if (flashPath[0] == '\r' || flashPath[0] == '\n')
				flashPath[0] = '\0';
			flashPathSz = wcslen(flashPath);
			while (flashPathSz && (flashPath[flashPathSz - 1] == '\r' || flashPath[flashPathSz - 1] == '\n'))
			{
				/* Trim flash path */
				flashPath[flashPathSz - 1] = '\0';
				--flashPathSz;
			}

			if (flashFile[0] == '\0' && flashPath[0] == '\0')
			{
				/* No flash */
				printf("\n");
				return;
			}

			/* Check if this file can be opened */
			ferr = _wfopen_s(&f, flashPath[0] ? flashPath : flashFile, L"rb");
			if (ferr || !f)
			{
				printf("File \"%ls\" cannot be opened\n", flashPath[0] ? flashPath : flashFile);
				goto SELECTFLASH;
			}
			fseek(f, 0, SEEK_END);
			fclose(f);
			f = NULL;
			printf("\n");
			if (!flashPath[0])
#pragma warning(push)
#pragma warning(disable : 4996)
				wcscpy(flashPath, flashFile);
#pragma warning(pop)
		}
	}
}
#endif

#if defined(BT8XXEMU_PLATFORM)
#if defined(ESD_SIMULATION) && defined(EVE_FLASH_AVAILABLE)
extern void Esd_SetFlashFirmware__ESD(const eve_tchar_t *path);
#endif
void EVE_Util_emulatorDefaults(EVE_HalParameters *params, void *emulatorParams, EVE_CHIPID_T chipId)
{
#if defined(EVE_MULTI_PLATFORM_TARGET)
	if (params->Host != EVE_HOST_BT8XXEMU)
		return;
#endif

	BT8XXEMU_EmulatorParameters *pEmulatorParams = emulatorParams;

	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, pEmulatorParams, chipId); // TODO: should be pEmulatorParams->mode?
	pEmulatorParams->Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);

	// TODO: emulatorParams.Log
	params->EmulatorParameters = pEmulatorParams;
}

#if defined(EVE_FLASH_AVAILABLE)
void EVE_Util_emulatorFlashDefaults(EVE_HalParameters *params, const void *emulatorParams, void *flashParams, const eve_tchar_t *flashPath)
{
	const BT8XXEMU_EmulatorParameters *pEmulatorParams = emulatorParams;
	BT8XXEMU_FlashParameters *pFlashParams = flashParams;

#if defined(EVE_MULTI_PLATFORM_TARGET)
	if (params->Host != EVE_HOST_BT8XXEMU)
		return;
#endif
	if (pEmulatorParams->Mode < BT8XXEMU_EmulatorBT815)
		return;
	if (!flashPath || !flashPath[0])
		return;

	BT8XXEMU_Flash_defaults(BT8XXEMU_VERSION_API, pFlashParams);
	wcscpy_s(pFlashParams->DataFilePath, _countof(pFlashParams->DataFilePath), flashPath);
#if defined(_DEBUG)
	pFlashParams->StdOut = 1;
#endif

#if defined(EVE_FLASH_FIRMWARE) /* only defined under ESD_SIMULATION */
	Esd_SetFlashFirmware__ESD(EVE_FLASH_FIRMWARE);
#endif

#ifdef EVE_FLASH_SIZE
	pFlashParams->SizeBytes = EVE_FLASH_SIZE * 1024 * 1024;
#else
	pFlashParams->SizeBytes = 2 * 1024 * 1024;
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
	FILE *f = _wfopen(flashPath, L"rb");
#pragma warning(pop)
#else
	FILE *f = fopen(flashPath, "rb");
#endif
	if (f)
	{
		fseek(f, 0, SEEK_END);
		int64_t flashSize = ftell(f);
		fclose(f);
		while (pFlashParams->SizeBytes < flashSize)
			pFlashParams->SizeBytes *= 2;
	}
#endif

	// TODO: flashParams.Log
	params->EmulatorFlashParameters = pFlashParams;
}
#endif

#endif

#if defined(_WIN32) && defined(EVE_FLASH_AVAILABLE)
#pragma warning(push)
#pragma warning(disable : 6262) // Large stack due to buffer
EVE_HAL_EXPORT void EVE_Util_uploadFlashFileInteractive(EVE_HalContext *phost, const eve_tchar_t *flashPath, bool updateFlashFirmware)
{
	ptrdiff_t flashSize;
	FILE *f = NULL;
	uint8_t buffer[64 * 4096];
	uint8_t rbuffer[64 * 4096];

	/* Upload flash */

	EVE_BootupParameters bootupParams;
	printf("Preparing to upload flash...\n");

	/* Open flash file and get size */
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
	f = _wfopen(flashPath, L"rb");
#pragma warning(pop)
#else
	f = fopen(flashPath, "rb");
#endif

	if (!f)
	{
		printf("Flash file cannot be opened\n");
	}
	else
	{

		fseek(f, 0, SEEK_END);
		flashSize = ftell(f);
		fseek(f, 0, SEEK_SET);

		/* Get the default bootup parameters for the device */
		EVE_Util_bootupDefaults(phost, &bootupParams);

		if (EVE_Util_bootup(phost, &bootupParams))
		{
			/* Get the default bootup parameters for the device */
			EVE_ConfigParameters configParams;
			EVE_Util_configDefaults(phost, &configParams, EVE_DISPLAY_DEFAULT);

			/* No display */
			configParams.PCLK = 0;

			/* Boot up */
			if (EVE_Util_config(phost, &configParams))
			{
				do
				{
					uint32_t flashStatus;
					uint32_t flashDeviceSize;
					size_t remainingBytes;
					uint32_t flashAddr;

					if (!EVE_Hal_supportFlash(phost))
					{
						printf("This device doesn't support flash\n");
						break;
					}

					flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
					if (flashStatus == FLASH_STATUS_DETACHED)
					{
						EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
						if (!EVE_Cmd_waitFlush(phost)) /* Wait for command completion */
						{
							printf("Coprocessor fault\n");
							break;
						}
					}

					flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
					if (flashStatus < FLASH_STATUS_BASIC)
					{
						printf("Flash could not be attached\n");
						break;
					}

					if (updateFlashFirmware)
					{
						printf("Upload flash firmware from image...\n");
						if (fread(buffer, 4096, 1, f) != 1)
						{
							printf("Could not read file\n");
							break;
						}
						EVE_Hal_wrMem(phost, 0, buffer, 4096);
						EVE_Cmd_startFunc(phost);
						EVE_Cmd_wr32(phost, CMD_FLASHUPDATE);
						EVE_Cmd_wr32(phost, 0);
						EVE_Cmd_wr32(phost, 0);
						EVE_Cmd_wr32(phost, 4096);
						EVE_Cmd_endFunc(phost);
						if (!EVE_Cmd_waitFlush(phost)) /* Wait for command completion */
						{
							printf("Coprocessor fault\n");
							break;
						}
					}
					else
					{
						if (fseek(f, 4096, SEEK_CUR))
						{
							printf("Could not seek file\n");
						}
					}

					if (flashStatus == FLASH_STATUS_BASIC)
					{
						uint32_t resAddr;
						printf("Enter fast flash mode\n");
						EVE_Cmd_startFunc(phost);
						EVE_Cmd_wr32(phost, CMD_FLASHFAST);
						resAddr = EVE_Cmd_moveWp(phost, 4); /* Get the address where the coprocessor will write the result */
						EVE_Cmd_endFunc(phost);
						if (!EVE_Cmd_waitFlush(phost)) /* Wait for command completion */
						{
							printf("Coprocessor fault\n");
							break;
						}
						EVE_Hal_rd32(phost, RAM_CMD + resAddr); /* Fetch result */
					}

					flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
					if (flashStatus < FLASH_STATUS_FULL)
					{
						printf("Flash could not enter fast mode. Has the correct firmware been uploaded?\n");
						break;
					}

					flashDeviceSize = EVE_Hal_rd32(phost, REG_FLASH_SIZE) * 1024 * 1024;
					if (flashDeviceSize < flashSize)
					{
						printf("Not enough space on flash, need %i bytes, have %i bytes\n", (int)flashSize, (int)flashDeviceSize);
						break;
					}

					remainingBytes = flashSize - 4096;
					flashAddr = 4096;

					printf("Upload flash from image...\n");
					eve_assert(sizeof(buffer) >= 4096);

					while (remainingBytes)
					{
						/* Read from file */
						size_t el;
						size_t sz;
						if (remainingBytes < 4096)
						{
							el = fread(buffer, 1, remainingBytes, f);
							sz = 4096;
							remainingBytes = 0;
						}
						else
						{
							el = fread(buffer, 4096, min(remainingBytes, sizeof(buffer)) / 4096, f);
							sz = el * 4096;
							remainingBytes -= sz;
						}
						if (!el)
						{
							printf("\nFailed to read file\n");
							break;
						}
						printf("B");

						/* Write to flash */
					REWRITE:
						EVE_Hal_wrMem(phost, 0, buffer, (uint32_t)sz);
						EVE_Cmd_startFunc(phost);
						EVE_Cmd_wr32(phost, CMD_FLASHUPDATE);
						EVE_Cmd_wr32(phost, flashAddr);
						EVE_Cmd_wr32(phost, 0);
						EVE_Cmd_wr32(phost, (uint32_t)sz);
						EVE_Cmd_endFunc(phost);
						if (!EVE_Cmd_waitFlush(phost)) /* Wait for command completion */
						{
							printf("\nCoprocessor fault\n");
							break;
						}
						printf("R");

						/* Verify using CMD_FLASHREAD */
						EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, 0);
						for (size_t i = 0; i < sz; ++i)
							EVE_Hal_transfer8(phost, 0x0F);
						EVE_Hal_endTransfer(phost);
						EVE_Cmd_startFunc(phost);
						EVE_Cmd_wr32(phost, CMD_FLASHREAD);
						EVE_Cmd_wr32(phost, 0);
						EVE_Cmd_wr32(phost, flashAddr);
						EVE_Cmd_wr32(phost, (uint32_t)sz);
						EVE_Cmd_endFunc(phost);
						if (!EVE_Cmd_waitFlush(phost)) /* Wait for command completion */
						{
							printf("\nCoprocessor fault\n");
							break;
						}
						EVE_Hal_rdMem(phost, rbuffer, 0, (uint32_t)sz);
						for (size_t i = 0; i < sz; ++i)
						{
							if (buffer[i] != rbuffer[i])
							{
								printf("\nVerification failed\n");
								goto REWRITE;
							}
						}
						printf("T");

						flashAddr += (uint32_t)sz;
					}

					if (!remainingBytes)
					{
						printf("\nFlash upload is ready\n");
					}
					printf("\n");
				} while (false); /* breakable scope */
			}
			else
			{
				printf("Failed to bootup the device\n");
			}

			/* Shutdown */
			EVE_Util_shutdown(phost);
		}
		else
		{
			printf("Failed to bootup the device\n");
		}
	}
	printf("\n");

	if (f)
	{
		fclose(f);
	}
}
#pragma warning(pop)
#endif

bool EVE_Util_openDeviceInteractive(EVE_HalContext *phost, const wchar_t *flashFile)
{
	EVE_CHIPID_T chipId;
	size_t deviceIdx;
	bool opened;

#ifdef BT8XXEMU_PLATFORM
	BT8XXEMU_EmulatorParameters emulatorParams;
#if defined(EVE_FLASH_AVAILABLE)
	BT8XXEMU_FlashParameters flashParams;
#endif
#endif
	EVE_HalParameters params = { 0 };
#if defined(_WIN32)
	bool updateFlash = false;
	bool updateFlashFirmware = false;
	wchar_t flashPath[MAX_PATH];
	flashPath[0] = '\0';
#endif

	/* Interactive device selection */
	EVE_Util_selectDeviceInteractive(&chipId, &deviceIdx);

	/* Fetch the default parameters for a device. Set the expected chip id.
	Pass the device index, or -1 to select the first device */
	EVE_Hal_defaultsEx(&params, deviceIdx);

#if defined(_WIN32) && defined(EVE_FLASH_AVAILABLE)
	if (chipId >= EVE_BT815 || (chipId <= 0 && flashFile && flashFile[0]))
		EVE_Util_selectFlashFileInteractive(flashPath, &updateFlash, &updateFlashFirmware, &params, flashFile);
#endif

#if defined(BT8XXEMU_PLATFORM)
	EVE_Util_emulatorDefaults(&params, &emulatorParams, chipId);
#if defined(EVE_FLASH_AVAILABLE)
	EVE_Util_emulatorFlashDefaults(&params, &emulatorParams, &flashParams, flashPath);
#endif
#endif

	opened = EVE_Hal_open(phost, &params);

	if (!opened)
		return false;

#if defined(_WIN32) && defined(EVE_FLASH_AVAILABLE)
	if (updateFlash && flashPath[0])
		EVE_Util_uploadFlashFileInteractive(phost, flashPath, updateFlashFirmware);
#endif

	return true;
}

/* Calls EVE_Util_bootup and EVE_Util_config using the default parameters.
Falls back to no interactivity on FT9XX platform */
EVE_HAL_EXPORT bool EVE_Util_bootupConfigInteractive(EVE_HalContext *phost, EVE_DISPLAY_T display)
{
	EVE_DISPLAY_T selectedDisplay;
	EVE_BootupParameters bootup;
	EVE_ConfigParameters config;

	EVE_Util_bootupDefaults(phost, &bootup);
	if (!EVE_Util_bootup(phost, &bootup))
	{
		return false;
	}

	EVE_Util_selectDisplayInteractive(&selectedDisplay);
	if (!selectedDisplay)
		selectedDisplay = display;

	EVE_Util_configDefaults(phost, &config, selectedDisplay);
	if (!EVE_Util_config(phost, &config))
	{
		EVE_Util_shutdown(phost);
		return false;
	}

	return true;
}

void EVE_Util_forceFault(EVE_HalContext *phost, const char *err)
{
	/* TODO: Clean this up a bit */
	if (!EVE_Cmd_waitSpace(phost, 0))
		return;
	if (!EVE_Util_resetCoprocessor(phost))
		return;
	EVE_CoCmd_dlStart(phost);
	for (int i = 0; i < EVE_DL_COUNT; ++i)
		EVE_Cmd_wr32(phost, DISPLAY());
	EVE_Cmd_waitFlush(phost);
	EVE_Cmd_wr32(phost, DISPLAY());
	EVE_Hal_flush(phost);
#if defined(_DEBUG)
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	EVE_sleep(100);
	if (EVE_CHIPID >= EVE_BT815 && (EVE_Cmd_rp(phost) & 0x3))
	{
		uint32_t errLen = (uint32_t)strlen(err);
		EVE_Hal_wrMem(phost, RAM_ERR_REPORT, (const uint8_t *)err, errLen);
		if (errLen < sizeof(phost->DebugBackup))
		{
			EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_ERR_REPORT + errLen);
			for (int i = (int)errLen; i < sizeof(phost->DebugBackup); ++i)
				EVE_Hal_transfer8(phost, 0);
			EVE_Hal_endTransfer(phost);
		}
	}
#endif
#endif
	EVE_Cmd_waitFlush(phost); /* Fault state now */
#if defined(_DEBUG)
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815
	    && phost->DebugMessageVisible)
	{
		uint32_t errLen = (uint32_t)strlen(err);
		EVE_Hal_wrMem(phost, RAM_G + RAM_G_SIZE - sizeof(phost->DebugBackup), (const uint8_t *)err, errLen);
		if (errLen < sizeof(phost->DebugBackup))
		{
			EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_G + RAM_G_SIZE - sizeof(phost->DebugBackup) + errLen);
			for (int i = (int)errLen; i < sizeof(phost->DebugBackup); ++i)
				EVE_Hal_transfer8(phost, 0);
			EVE_Hal_endTransfer(phost);
		}
	}
#endif
#endif
}

/* end of file */
