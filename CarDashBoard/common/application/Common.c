/**
 * @file Common.c
 * @brief Common functions
 *
 * @author Tuan Nguyen <tuan.nguyen@brtchip.com>
 *
 * @date 30 Sep 2019
 * 
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Common.h"
#include <math.h>

char textbuffer[1000]; // for Draw_Text_Format
static uint32_t a;
static uint32_t b;
static uint32_t c;
static uint32_t d;
static uint32_t e;
static uint32_t f;

uint32_t VertextFormat;
uint32_t VertextPrecision;

/**
 * @brief Setup vertex format and precision
 * 
 * @param format Vertext format
 */
void Setup_Precision(uint32_t format) {
	VertextFormat = format;

	switch (VertextFormat) {
	case 0:
		VertextPrecision = 1;
		break;
	case 1:
		VertextPrecision = 2;
		break;
	case 2:
		VertextPrecision = 4;
		break;
	case 3:
		VertextPrecision = 8;
		break;
	case 4:
		VertextPrecision = 16;
		break;
	default:
		VertextPrecision = 16;
		break;
	}
}

/**
 * @brief Callback function
 * 
 * @param phost 
 * @return true 
 */
bool cbCmdWait(struct EVE_HalContext* phost) {
	(void) (phost); // suspress warning on unused
	return true;
}

/**
 * @brief Capacitive Touch initialzation
 * 
 * @param phost 
 */
void Init_CapacitiveTouch(EVE_HalContext* phost) {
#if defined(EVE_SUPPORT_CAPACITIVE)
	char touch_capcs_815[] = { 0xD7, 0x02, 0xD8, 0x02, 0x00, 0x80, 0x8C, 0x66,
			0x01, 0x80, 0x8F, 0x62, 0x27, 0x61, 0x10, 0x61, 0x1D, 0x6B, 0x9C,
			0x61, 0x00, 0x80, 0x8F, 0x67, 0x10, 0x61, 0x9D, 0x61, 0x11, 0x61,
			0x9D, 0x61, 0x27, 0x61, 0x8C, 0x60, 0x27, 0x61, 0x11, 0x61, 0x1D,
			0x6B, 0x9C, 0x61, 0x27, 0x61, 0x27, 0x61, 0x11, 0x61, 0x11, 0x61,
			0x1D, 0x6B, 0x1D, 0x6B, 0x27, 0x61, 0x10, 0x61, 0x27, 0x61, 0x10,
			0x61, 0x1D, 0x6B, 0x1D, 0x6B, 0x10, 0x61, 0x27, 0x61, 0x10, 0x61,
			0x1D, 0x6B, 0x8C, 0x60, 0x03, 0x67, 0x8C, 0x66, 0x0A, 0x40, 0x8C,
			0x66, 0x00, 0x80, 0x8F, 0x68, 0x10, 0x61, 0x8F, 0x68, 0x03, 0x68,
			0x8C, 0x66, 0x10, 0x61, 0x8F, 0x6F, 0x00, 0x6A, 0x8C, 0x66, 0x33,
			0x40, 0x8F, 0x62, 0x0E, 0x40, 0x03, 0x68, 0x3C, 0x20, 0x03, 0x61,
			0x3D, 0x00, 0x8F, 0x60, 0x8C, 0x60, 0x0E, 0x40, 0x2D, 0x40, 0x39,
			0x00, 0x11, 0x60, 0x8F, 0x62, 0x01, 0x80, 0x8F, 0x69, 0x00, 0x6A,
			0x00, 0x80, 0x8C, 0x67, 0x27, 0x80, 0x01, 0x80, 0x10, 0x61, 0x33,
			0x60, 0x8F, 0x61, 0x00, 0x80, 0x4A, 0x00, 0x11, 0x6C, 0x10, 0x61,
			0x04, 0x40, 0x8C, 0x6C, 0x06, 0x40, 0x0B, 0x80, 0x4B, 0x40, 0x4B,
			0x00, 0x4F, 0x40, 0x10, 0x61, 0x27, 0x61, 0x03, 0x62, 0x10, 0x61,
			0x11, 0x6B, 0x03, 0x62, 0x10, 0x61, 0x11, 0x61, 0x1D, 0x6B, 0x03,
			0x6F, 0x35, 0x00, 0x4F, 0x40, 0x06, 0x40, 0x27, 0x61, 0x00, 0x80,
			0x00, 0x80, 0x1D, 0x6B, 0x11, 0x60, 0x78, 0x20, 0x24, 0x60, 0x01,
			0x80, 0x03, 0x63, 0x71, 0x20, 0x16, 0x40, 0x58, 0x40, 0x1C, 0x40,
			0x0E, 0x40, 0x58, 0x40, 0x1C, 0x40, 0x1D, 0x6B, 0x43, 0x40, 0x69,
			0x00, 0x03, 0x61, 0x1C, 0x40, 0x03, 0x61, 0x8F, 0x61, 0x0E, 0x40,
			0x14, 0x80, 0x63, 0x40, 0x27, 0x61, 0x27, 0x61, 0x12, 0x80, 0x63,
			0x40, 0x1D, 0x6B, 0x1D, 0x6B, 0x58, 0x40, 0x16, 0x80, 0x57, 0x40,
			0x03, 0x60, 0x22, 0x40, 0x1A, 0x80, 0x63, 0x40, 0x27, 0x61, 0x27,
			0x61, 0x18, 0x80, 0x63, 0x40, 0x1D, 0x6B, 0x1D, 0x6B, 0x58, 0x40,
			0x1C, 0x80, 0x57, 0x40, 0x8F, 0x60, 0x01, 0x80, 0x10, 0x61, 0x09,
			0x80, 0x00, 0x6C, 0x10, 0x61, 0x03, 0x69, 0x8F, 0x63, 0x0C, 0x80,
			0x00, 0x6C, 0x8F, 0x67, 0x00, 0x80, 0x9D, 0x40, 0x00, 0x66, 0xA0,
			0x20, 0x8C, 0x60, 0x00, 0x80, 0x0A, 0x80, 0x11, 0x6C, 0xA7, 0x20,
			0x33, 0x60, 0x8F, 0x61, 0x11, 0x60, 0xB4, 0x20, 0x40, 0x86, 0xA5,
			0x40, 0x45, 0x40, 0xAE, 0x20, 0x03, 0x61, 0x00, 0x6A, 0xAB, 0x00,
			0x8F, 0x61, 0xA5, 0x40, 0xA5, 0x00, 0x2C, 0x80, 0x49, 0x00, 0x2B,
			0x80, 0x4D, 0x40, 0x01, 0x80, 0x2A, 0x80, 0x33, 0x60, 0x03, 0x61,
			0x2A, 0x80, 0x11, 0x6C, 0x0A, 0x40, 0xC5, 0x20, 0xA5, 0x40, 0xC0,
			0x00, 0x8F, 0x61, 0x01, 0x80, 0x2B, 0x80, 0x33, 0x60, 0x8F, 0x61,
			0x2B, 0x80, 0x49, 0x40, 0xBB, 0x00, 0x2B, 0x80, 0x4D, 0x40, 0x00,
			0x80, 0x2A, 0x80, 0x33, 0x60, 0x8F, 0x61, 0x2B, 0x80, 0x49, 0x40,
			0xCF, 0x00, 0xCF, 0x40, 0xA5, 0x00, 0xCA, 0x40, 0xB5, 0x40, 0xB9,
			0x40, 0xB5, 0x40, 0xCD, 0x00, 0xD3, 0x40, 0xB5, 0x40, 0xCD, 0x40,
			0xB5, 0x40, 0xB9, 0x40, 0xB5, 0x40, 0xCA, 0x00, 0xC6, 0x40, 0xA5,
			0x40, 0xBB, 0x40, 0x2A, 0x80, 0x11, 0x6C, 0xE8, 0x20, 0x03, 0x61,
			0x2B, 0x80, 0xB5, 0x40, 0x00, 0x6C, 0xD6, 0x00, 0xCF, 0x40, 0x2B,
			0x80, 0x33, 0x60, 0x03, 0x61, 0xA5, 0x40, 0xBB, 0x40, 0xB5, 0x40,
			0xD6, 0x00, 0x7F, 0x80, 0x04, 0x80, 0x98, 0x00, 0x00, 0x80, 0xF7,
			0x40, 0x41, 0x40, 0x03, 0x64, 0xD8, 0x40, 0xCF, 0x40, 0xB5, 0x40,
			0x11, 0x60, 0x07, 0x80, 0x03, 0x69, 0xEF, 0x40, 0x11, 0x60, 0x06,
			0x80, 0x03, 0x69, 0xEF, 0x40, 0x11, 0x60, 0x05, 0x80, 0x03, 0x69,
			0xEF, 0x40, 0x11, 0x60, 0x04, 0x80, 0x03, 0x69, 0xEF, 0x40, 0x11,
			0x60, 0x03, 0x80, 0x03, 0x69, 0xEF, 0x40, 0x11, 0x60, 0x02, 0x80,
			0x03, 0x69, 0xEF, 0x40, 0x11, 0x60, 0x01, 0x80, 0x03, 0x69, 0xEF,
			0x40, 0xEF, 0x40, 0xE4, 0x40, 0x21, 0x21, 0x00, 0x00, 0x8C, 0x60,
			0x41, 0x40, 0xE4, 0x40, 0x8F, 0x62, 0xE4, 0x40, 0x22, 0x41, 0x22,
			0x41, 0x22, 0x41, 0x22, 0x41, 0x22, 0x41, 0x22, 0x41, 0x22, 0x01,
			0x01, 0x80, 0xFB, 0x40, 0x25, 0x41, 0x00, 0x80, 0xEF, 0x00, 0x02,
			0x40, 0xEF, 0x00, 0x25, 0x41, 0x30, 0x01, 0x34, 0x41, 0x22, 0x41,
			0x22, 0x41, 0x22, 0x41, 0x22, 0x41, 0x22, 0x41, 0x22, 0x41, 0x22,
			0x41, 0x22, 0x41, 0x30, 0x01, 0x25, 0x41, 0x32, 0x41, 0x03, 0x61,
			0xDD, 0x00, 0x22, 0x80, 0x53, 0x00, 0x20, 0x80, 0x53, 0x00, 0x24,
			0x80, 0x53, 0x00, 0x21, 0x80, 0x4B, 0x40, 0x44, 0x41, 0x0E, 0x40,
			0x46, 0x41, 0x23, 0x80, 0x53, 0x00, 0x1D, 0x6B, 0x03, 0x62, 0x27,
			0x61, 0x8C, 0x60, 0xFF, 0xFF, 0x00, 0x66, 0x9D, 0x60, 0x55, 0x41,
			0x06, 0x40, 0x04, 0x80, 0x37, 0x40, 0x51, 0x41, 0x44, 0x01, 0x46,
			0x01, 0x4F, 0x01, 0x48, 0x01, 0x21, 0x80, 0x4B, 0x40, 0x25, 0x80,
			0x4B, 0x00, 0x05, 0x80, 0x11, 0x60, 0x00, 0x6A, 0x58, 0x41, 0x45,
			0x40, 0x66, 0x21, 0x8C, 0x60, 0x11, 0x60, 0x05, 0x80, 0x03, 0x6F,
			0x75, 0x21, 0x01, 0x80, 0x10, 0x61, 0x03, 0x71, 0x03, 0x64, 0x76,
			0x01, 0x8F, 0x61, 0x8C, 0x60, 0x27, 0x61, 0x00, 0x80, 0x10, 0x61,
			0x11, 0x60, 0x82, 0x21, 0x10, 0x61, 0x11, 0x6B, 0x10, 0x40, 0x10,
			0x61, 0x00, 0x6A, 0x7A, 0x01, 0xB5, 0x40, 0x40, 0x41, 0x03, 0x61,
			0x0C, 0x60, 0x11, 0x60, 0x00, 0x80, 0x11, 0x60, 0x05, 0x80, 0x03,
			0x68, 0x98, 0x21, 0x27, 0x61, 0x11, 0x60, 0x01, 0x80, 0x03, 0x63,
			0x0A, 0x40, 0x94, 0x21, 0x11, 0x6B, 0x58, 0x41, 0x43, 0x40, 0x1D,
			0x6B, 0x04, 0x40, 0x88, 0x01, 0x7A, 0x00, 0x03, 0x80, 0x96, 0x40,
			0x0A, 0x00, 0xF7, 0x40, 0x5D, 0x80, 0x8F, 0x67, 0x4E, 0x81, 0xFA,
			0x40, 0x9C, 0x41, 0xA8, 0x21, 0x08, 0x80, 0x00, 0x69, 0x80, 0x80,
			0x03, 0x64, 0x01, 0x41, 0x01, 0x01, 0x00, 0x80, 0x0E, 0x80, 0x9E,
			0x00, 0xA9, 0x41, 0x8C, 0x66, 0xB5, 0x40, 0x11, 0x61, 0x10, 0x40,
			0x00, 0x80, 0x9D, 0x40, 0x03, 0x64, 0xB6, 0x21, 0x01, 0x00, 0x2D,
			0x80, 0x00, 0x6C, 0x00, 0x67, 0xAF, 0x21, 0x7A, 0x00, 0xAC, 0x81,
			0x9C, 0x41, 0xC4, 0x21, 0x4D, 0x80, 0xA0, 0x41, 0xDD, 0x40, 0x2D,
			0x41, 0x40, 0x41, 0xC5, 0x01, 0x99, 0x41, 0x01, 0x80, 0x03, 0x63,
			0x0E, 0x40, 0xAE, 0x41, 0x01, 0x80, 0x03, 0x65, 0xAE, 0x01, 0xA9,
			0x81, 0xBC, 0x01, 0x36, 0x41, 0xFF, 0x8F, 0x8F, 0x63, 0xCE, 0x41,
			0x36, 0x41, 0x0C, 0x80, 0x00, 0x69, 0x27, 0x61, 0xCF, 0x41, 0x36,
			0x41, 0x03, 0x61, 0x7C, 0x40, 0x11, 0x6B, 0x5A, 0x41, 0x1D, 0x6B,
			0x6C, 0x01, 0xFA, 0x40, 0x02, 0x80, 0x01, 0x41, 0xDD, 0x40, 0x2D,
			0x01, 0xDE, 0x41, 0xD1, 0x81, 0x77, 0x41, 0x8F, 0x61, 0x02, 0x40,
			0x02, 0x40, 0x55, 0x41, 0xFF, 0xFF, 0x8C, 0x60, 0xDE, 0x41, 0xCE,
			0x41, 0xCE, 0x41, 0x06, 0x40, 0xF5, 0x21, 0x0E, 0x40, 0x7C, 0x40,
			0xC8, 0x80, 0xF7, 0x01, 0x7A, 0x40, 0xE7, 0x41, 0xB5, 0x40, 0x40,
			0x41, 0x4A, 0x01, 0xBB, 0x41, 0xE3, 0x41, 0x48, 0x40, 0xFA, 0x01,
			0xCC, 0x41, 0xEC, 0x41, 0x48, 0x40, 0xFE, 0x01, 0x34, 0x41, 0x34,
			0x41, 0x08, 0x80, 0x03, 0x71, 0x8F, 0x62, 0x9F, 0x41, 0xDD, 0x40,
			0x2D, 0x41, 0x0F, 0x80, 0x8F, 0x63, 0x07, 0x42, 0x34, 0x41, 0x03,
			0x61, 0x02, 0x42, 0x02, 0x42, 0xEF, 0x41, 0x9F, 0x41, 0x00, 0x80,
			0x01, 0x41, 0xDD, 0x00, 0xCC, 0x41, 0x0C, 0x42, 0x48, 0x40, 0x16,
			0x02, 0x34, 0x41, 0x27, 0x61, 0x02, 0x42, 0x02, 0x42, 0x34, 0x41,
			0x03, 0x61, 0xD7, 0x01, 0x07, 0x42, 0x1A, 0x82, 0x77, 0x41, 0x12,
			0x02, 0x00, 0x80, 0xBB, 0x41, 0x21, 0x42, 0x0C, 0x40, 0x03, 0x64,
			0x2C, 0x22, 0x48, 0x40, 0x26, 0x02, 0x27, 0x61, 0x11, 0x6B, 0x00,
			0x6C, 0x11, 0x67, 0x34, 0x22, 0x03, 0x61, 0x2E, 0x02, 0x0C, 0x60,
			0x8F, 0x60, 0x11, 0x64, 0x2B, 0x00, 0x01, 0x80, 0x11, 0x61, 0x8F,
			0x71, 0x00, 0x80, 0x11, 0x60, 0x05, 0x80, 0x03, 0x68, 0x4C, 0x22,
			0x27, 0x61, 0x11, 0x60, 0x01, 0x80, 0x03, 0x63, 0x0A, 0x40, 0x48,
			0x22, 0x11, 0x6B, 0x58, 0x41, 0x43, 0x40, 0x1D, 0x6B, 0x04, 0x40,
			0x3C, 0x02, 0x7A, 0x00, 0xAC, 0x41, 0x52, 0x22, 0xE7, 0x41, 0x4A,
			0x41, 0x53, 0x02, 0x65, 0x41, 0x29, 0x80, 0x49, 0x40, 0x0F, 0x80,
			0x10, 0x80, 0x00, 0x6C, 0x11, 0x67, 0x5C, 0x22, 0x03, 0x61, 0x56,
			0x02, 0x0F, 0x80, 0x00, 0x65, 0x68, 0x22, 0x0D, 0x80, 0x00, 0x6C,
			0x11, 0x80, 0x00, 0x6C, 0x27, 0x61, 0x27, 0x61, 0x29, 0x80, 0x4D,
			0x40, 0x56, 0x02, 0x03, 0x61, 0xAC, 0x41, 0x76, 0x22, 0x1D, 0x6B,
			0x1D, 0x6B, 0x36, 0x42, 0x71, 0x22, 0xE7, 0x41, 0x74, 0x02, 0x0E,
			0x40, 0x7C, 0x40, 0xC8, 0x80, 0x4A, 0x41, 0x8D, 0x02, 0x00, 0x80,
			0x10, 0x61, 0x0F, 0x80, 0x00, 0x65, 0x8B, 0x22, 0x38, 0x42, 0x06,
			0x40, 0x03, 0x64, 0x10, 0x61, 0x1D, 0x6B, 0x1D, 0x6B, 0x36, 0x42,
			0x86, 0x22, 0x7A, 0x40, 0x55, 0x41, 0x87, 0x02, 0x7C, 0x40, 0x06,
			0x40, 0x5A, 0x41, 0x10, 0x61, 0x78, 0x02, 0x03, 0x61, 0x3B, 0x42,
			0x48, 0x40, 0x53, 0x02, 0x8C, 0x60, 0x0B, 0x80, 0x96, 0x40, 0xA0,
			0x20, 0xFA, 0x40, 0x99, 0x41, 0x98, 0x22, 0x03, 0x80, 0xA5, 0x80,
			0x01, 0x41, 0x01, 0x41, 0xDD, 0x40, 0xA0, 0x40, 0x00, 0x00, 0x2C,
			0x80, 0x4D, 0x40, 0xAB, 0x40, 0xB7, 0x40, 0xAB, 0x00, 0x0E, 0x80,
			0x96, 0x40, 0xA6, 0x22, 0x4D, 0x42, 0xB7, 0x40, 0xAC, 0x41, 0x11,
			0x60, 0xAD, 0x22, 0xE7, 0x41, 0x4A, 0x41, 0xAE, 0x02, 0x65, 0x41,
			0x48, 0x40, 0x02, 0x80, 0x9A, 0x41, 0xC2, 0x22, 0x9C, 0x41, 0xBF,
			0x22, 0x01, 0x80, 0x04, 0x80, 0x4B, 0x40, 0x37, 0x80, 0x01, 0x80,
			0x9D, 0x42, 0x04, 0x80, 0x4D, 0x40, 0x50, 0x80, 0xAB, 0x40, 0xC2,
			0x02, 0x2C, 0x81, 0x06, 0x80, 0x9D, 0x42, 0xAC, 0x41, 0x27, 0x40,
			0x1F, 0x41, 0x01, 0x80, 0xAB, 0x00, 0x00, 0x80, 0x9D, 0x40, 0xCB,
			0x22, 0x90, 0x42, 0x9C, 0x41, 0xD0, 0x22, 0xAC, 0x41, 0x25, 0x22,
			0x16, 0x42, 0xAC, 0x41, 0xFA, 0x21, 0xFE, 0x41, 0xFA, 0x40, 0xA1,
			0x41, 0x01, 0x41, 0xDD, 0x00, 0xA2, 0x42, 0xC7, 0x02 };
	EVE_sleep(1000);

	EVE_Hal_wr8(phost, REG_CPURESET, 2);
	EVE_Hal_wrMem(phost, RAM_JTBOOT, touch_capcs_815, sizeof(touch_capcs_815));
	EVE_Hal_wr8(phost, REG_CPURESET, 0);
#endif
}

/**
 * @brief EVE initialzation
 * 
 * @param phost 
 */
void Gpu_Init(EVE_HalContext* phost) {
	EVE_CHIPID_T chipId;
	size_t deviceIdx;

	EVE_HalParameters params;

#if defined(BT8XXEMU_PLATFORM)
	printf(BT8XXEMU_version());
	BT8XXEMU_EmulatorParameters emulatorParams;
#if defined(EVE_FLASH_AVAILABLE)
	BT8XXEMU_FlashParameters flashParams;
#endif
#endif

	EVE_Hal_initialize();

#ifdef EVE_MULTI_TARGET
	EVE_Util_selectDeviceInteractive(&chipId, &deviceIdx);
#else
	chipId = EVE_SUPPORT_CHIPID;
	deviceIdx = -1;
#endif

	EVE_Hal_defaultsEx(&params, deviceIdx);
	params.CbCmdWait = cbCmdWait;

#ifdef BT8XXEMU_PLATFORM
	EVE_Util_emulatorDefaults(&params, &emulatorParams, chipId);
#if defined(EVE_FLASH_AVAILABLE)
	EVE_Util_emulatorFlashDefaults(&params, &emulatorParams, &flashParams, L"__Flash.bin");
#endif
#endif

	EVE_Hal_open(phost, &params);

#ifdef EVE_MULTI_TARGET
	EVE_Util_bootupConfigInteractive(phost, EVE_DISPLAY_DEFAULT);
#else
	EVE_Util_bootupConfig(phost);
#endif

#ifdef DISPLAY_RESOLUTION_WXGA
	EVE_Hal_wr8(phost, REG_ADAPTIVE_FRAMERATE, 0);
	EVE_Hal_wr8(phost, REG_CPURESET, 2);
	EVE_Hal_wr16(phost, REG_TOUCH_CONFIG, 0x05D0);	//for Goodix touch panel
	EVE_Hal_wr8(phost, REG_CPURESET, 0);
	EVE_sleep(300);
#endif

#if defined(BT817_ENABLE) || defined(BT818_ENABLE)
	/// Enable capacity touch
	Init_CapacitiveTouch(phost);

	/// set API level to BT817
	EVE_CoCmd_apiLevel(phost, 2);
	EVE_Cmd_waitFlush(phost);
	printf("Set APILEVEL to BT817/8 (2)\n");
#else
	printf("Set APILEVEL to default (1)\n");
#endif

#if defined(EMBEDDED_PLATFORM)
	EVE_Util_loadSdCard(phost);
#endif

	// Precision control
	Setup_Precision(4); // Default vertext format = 4
}

void Gpu_Release(EVE_HalContext* phost) {
	EVE_Hal_close(phost);
	EVE_Hal_release();
}

/**
 * @brief Get system frequency in Hz
 * 
 * @param phost EVE_HalContext* pointer
 * @return uint32_t System clock requency in Hz
 */
uint32_t Get_SystemClock(EVE_HalContext* phost) {
	EVE_Hal_rd32(phost, REG_CLOCK); // warm up

	uint32_t c0 = EVE_Hal_rd32(phost, REG_CLOCK);
	uint32_t c1 = EVE_Hal_rd32(phost, REG_CLOCK);

	uint32_t overhead = c1 - c0;

	c0 = EVE_Hal_rd32(phost, REG_CLOCK);
	EVE_sleep(1000);
	c1 = EVE_Hal_rd32(phost, REG_CLOCK);

	uint32_t freq = (uint32_t) round((c1 - c0) - overhead);

	return freq;
}

/**
 * @brief Helper function to draw a point
 * 
 * @param phost 
 * @param Rx 
 * @param Ry 
 * @param R 
 * @return uint32_t 
 */
uint32_t Draw_Point(EVE_HalContext* phost, uint32_t Rx, uint32_t Ry, uint32_t R) {
	EVE_Cmd_wr32(phost, POINT_SIZE(VP(R)));
	EVE_Cmd_wr32(phost, BEGIN(POINTS));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(Rx), VP(Ry)));
	return 1;
}

uint32_t Draw_Image(EVE_HalContext* phost, const char* filePath,
		uint32_t format) {
#if defined(BT817_ENABLE) || defined(BT818_ENABLE)
	Gpu_Hal_LoadImageToMemory(phost, filePath, RAM_G, LOADIMAGE);
	uint32_t ptr = 0;
	uint32_t w;
	uint32_t h;
	EVE_CoCmd_getProps(phost, &ptr, &w, &h);
	Display_End(phost);

	EVE_CoCmd_dlStart(phost);
	EVE_Cmd_wr32(phost, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(phost, COLOR_RGB(255, 255, 255));

	EVE_Cmd_waitFlush(phost);
	EVE_CoCmd_setBitmap(phost, RAM_G, (uint16_t) format, (uint16_t) w,
			(uint16_t) h);

	//Start drawing bitmap
	EVE_Cmd_wr32(phost, BEGIN(BITMAPS));
	EVE_Cmd_wr32(phost, VERTEX2II(0, 0, 0, 0));
	EVE_Cmd_wr32(phost, END());
	EVE_Cmd_wr32(phost, RESTORE_CONTEXT());
	EVE_Cmd_wr32(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);
#endif
	return 1;
}

void Calibration_Restore(EVE_HalContext* phost) {
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_A, a);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_B, b);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_C, c);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_D, d);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_E, e);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_F, f);
}

void Calibration_Save(EVE_HalContext* phost) {
	a = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_A);
	b = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_B);
	c = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_C);
	d = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_D);
	e = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_E);
	f = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_F);
}

void Display_StartColor(EVE_HalContext* phost, uint8_t *bgColor, uint8_t *textColor) {
	EVE_CoCmd_dlStart(phost);
	EVE_Cmd_wr32(phost, CLEAR_COLOR_RGB(bgColor[0], bgColor[1], bgColor[2]));
	EVE_Cmd_wr32(phost, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(phost, COLOR_RGB(textColor[0], textColor[1], textColor[2]));
	EVE_Cmd_wr32(phost, VERTEX_FORMAT(VertextFormat));
}

void Display_Start(EVE_HalContext* phost) {
	Display_StartColor(phost, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 });
}

void Display_End(EVE_HalContext* phost) {
	EVE_Cmd_wr32(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);
}

static void Gpu_Text(EVE_HalContext* phost, const char8_t* str, uint8_t *bgColor, uint8_t *textColor) {
	uint16_t font = 0;

	if (DispWidth == 480) {
		font = 28;
	} else if (DispWidth == 800) {
		font = 30;
	} else {
		font = 27;
	}

	Display_StartColor(phost, bgColor, textColor);
	EVE_CoCmd_fillWidth(phost, DispWidth);
	EVE_CoCmd_text(phost, (uint16_t) (DispWidth / 2),
			(uint16_t) (DispHeight / 2), font, OPT_CENTERX | OPT_FILL, str);
	Display_End(phost);
}

void Draw_TextColor(EVE_HalContext* phost, const char8_t* str, uint8_t *bgColor, uint8_t *textColor) {
	Gpu_Text(phost, str, bgColor, textColor);
	EVE_sleep(2000);
}

void Draw_Text(EVE_HalContext* phost, const char8_t* str) {
	Gpu_Text(phost, str, (uint8_t[]) { 0xff, 0xff, 0xff }, (uint8_t[]) { 0, 0, 0});
	EVE_sleep(2000);
}

void Draw_Text2(EVE_HalContext* phost, const char8_t* str, int ms) {
	Gpu_Text(phost, str, (uint8_t[]) { 0xff, 0xff, 0xff }, (uint8_t[]) { 0, 0, 0 });
	EVE_sleep(ms);
}

void Play_MuteSound(EVE_HalContext* phost) {
	EVE_Hal_wr16(phost, REG_SOUND, 0x0060);
	EVE_Hal_wr8(phost, REG_PLAY, 0x01);
}

void Get_VC1Dump(EVE_HalContext* phost, const char* filename) {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#pragma warning(push)
#pragma warning(disable : 4996)
	FILE* vc1dump = fopen(filename, "wb");
#pragma warning(pop)

	if (!vc1dump) {
		APP_ERR("Cannot open file %s", filename);
		return;
	}

	const uint32_t version = 110;
	const uint32_t w = DispWidth;
	const uint32_t h = DispHeight;
	const uint32_t macro0 = 0;
	const uint32_t macro1 = 0;
	const uint32_t CRC_IMAGE = 0;

	fwrite(&version, 4, 1, vc1dump);
	fwrite(&w, 4, 1, vc1dump);
	fwrite(&h, 4, 1, vc1dump);
	fwrite(&macro0, 4, 1, vc1dump);
	fwrite(&macro1, 4, 1, vc1dump);
	fwrite(&CRC_IMAGE, 4, 1, vc1dump);

	uint8_t* buffer = malloc(RAM_G_SIZE);
	if (!buffer) {
		APP_ERR("Malloc failed");
		fclose(vc1dump);
		return;
	}

	memset(buffer, 0, RAM_G_SIZE);
	EVE_Hal_rdMem(phost, buffer, RAM_G, RAM_G_SIZE);
	fwrite(buffer, 1, RAM_G_SIZE, vc1dump);

	memset(buffer, 0, RAM_G_SIZE);
	EVE_Hal_rdMem(phost, buffer, RAM_DL, EVE_DL_SIZE);
	fwrite(buffer, 1, EVE_DL_SIZE, vc1dump);

	fclose(vc1dump);
	free(buffer);
#endif
}

void Play_Sound(EVE_HalContext* phost, uint8_t sound, uint8_t vol, uint8_t midi) {
	uint16_t val = (uint16_t) ((midi << 8) | sound);
	EVE_Hal_wr8(phost, REG_VOL_SOUND, vol);
	EVE_Hal_wr16(phost, REG_SOUND, val);
	EVE_Hal_wr8(phost, REG_PLAY, 1);
}

uint8_t isSDcardDetected() {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	return true;
#elif defined(FT9XX_PLATFORM)
	return sdhost_card_detect() == SDHOST_CARD_INSERTED;
#else
	return true;
#endif
}

void hintScreen(EVE_HalContext* phost, const uint8_t* msg) {
	Draw_Text(phost, msg);
}

void Flash_Init(EVE_HalContext* phost, const uint8_t *filePath,
		const uint8_t *fileName) {
#if defined(_WIN32)
#define _WHERE "PC"
#elif defined(EMBEDDED_PLATFORM)
#define _WHERE "SDcard"
#endif

	EVE_Util_loadSdCard(phost);
#if !defined(BT8XXEMU_PLATFORM) && defined(EVE_FLASH_AVAILABLE)
	/// if SD card is NOT detected, proceeds to boot up with a hint screen:
	if (!isSDcardDetected()) {
		return;
	} else {
		/// if SD card is detected, show a dialog on the screen with two options: Yes or No?
		if (!Show_Diaglog_YesNo(phost, "Flash programming",
				"Program the flash with the file in " _WHERE "?")) {
			/// If No, proceeds to boot up
			return;
		}

		/// If YES, Program the flash with the file in SD card
		int sent = Ftf_Write_File_To_Flash_With_Progressbar(phost, filePath,
				fileName, 0);

		/// If fail to program flash, reset application
		if (0 >= sent) {
			uint8_t count = 5;
			uint8_t msg[1000];
			while (count) {
#if defined(FT9XX_PLATFORM) || defined(EVE_PLATFORM_RP2040)
				snprintf(msg, 1000, "Error: Cannot open file: %s, reset in %us",
						filePath, count);
#else
				snprintf(msg, 1000, "Error: Cannot open file: %s, exit in %u s",
					fileName, count);
#endif
				Show_Diaglog_Info(phost, msg);
				EVE_sleep(1000);
				count--;
			}
			exit(0);
		}
	}
#endif
}

void Show_Diaglog_Info(EVE_HalContext* phost, const uint8_t* msg) {
	uint8_t ret = INPUT_NO;
	const uint32_t tag_y = 1;
	const uint32_t tag_n = 2;

	const uint16_t border = 2; // pixel
	const uint16_t base_w = 800;
	const uint16_t base_h = 600;
	uint16_t w = 400;
	uint16_t h = 200;
	uint16_t x = (base_w - w) / 2;
	uint16_t y = (base_h - h) / 2;
	uint8_t htop = 50;
	uint8_t hbottom = 50;
	uint8_t font = 20;

	uint16_t btn_h = 40;
	uint16_t btn_w = 100;

	uint32_t ratio = base_w * 100 / DispWidth;

	if (ratio != 100) {
		w = w * 100 / ratio;
		h = h * 100 / ratio;
		btn_w = btn_w * 100 / ratio;
		btn_h = btn_h * 100 / ratio;
		x = (DispWidth - w) / 2;
		y = (DispHeight - h) / 2;
		htop = htop * 100 / ratio;
		hbottom = hbottom * 100 / ratio;

		ratio += (100 - ratio) / 2;
		font = font * 100 / ratio;
		if (font > 31)
			font = 31;
		if (font < 16)
			font = 16;
		if (font == 17 || font == 19)
			font = 18;
	}

	Display_Start(phost);

	/// fade the whole LCD
	EVE_Cmd_wr32(phost, COLOR_RGB(0x3F, 0x3F, 0x3F));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, VERTEX2F(0, 0));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(DispWidth), VP(DispHeight)));

	/// diag border
	EVE_Cmd_wr32(phost, COLOR_RGB(0xE1, 0xE1, 0xE1));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x), VP(y)));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w), VP(y + h)));

	/// center part
	EVE_Cmd_wr32(phost, COLOR_RGB(0xFF, 0xFF, 0xFF));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + border), VP(y + border)));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w - border), VP(y + h - border)));

	/// top part
	EVE_Cmd_wr32(phost, COLOR_RGB(0xF3, 0xF3, 0xF3));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + border), VP(y + border)));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w - border), VP(y + htop)));

	/// bottom part
	EVE_Cmd_wr32(phost, COLOR_RGB(0xF3, 0xF3, 0xF3));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + border), VP(y + h - hbottom + border)));
	EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w - border), VP(y + h - border)));

	font = font * 8 / 10;
	if (font < 16)
		font = 16;
	if (font == 17 || font == 19)
		font = 18;
	EVE_Cmd_wr32(phost, COLOR_RGB(0x78, 0x78, 0x78));
	EVE_CoCmd_text(phost, x + border + 30, y + h / 2 - 20, font, 0, msg);

	Display_End(phost);

}

uint8_t Show_Diaglog_YesNo(EVE_HalContext* phost, const uint8_t* title,
		const uint8_t* msg) {
	uint8_t ret = INPUT_NO;
	const uint32_t tag_y = 1;
	const uint32_t tag_n = 2;

	const uint16_t border = 2; // pixel
	const uint16_t base_w = 800;
	const uint16_t base_h = 600;
	uint16_t w = 400;
	uint16_t h = 200;
	uint16_t x = (base_w - w) / 2;
	uint16_t y = (base_h - h) / 2;
	uint8_t htop = 50;
	uint8_t hbottom = 50;
	uint8_t font = 20;

	uint16_t btn_h = 30;
	uint16_t btn_w = 80;

	uint32_t ratio = base_w * 100 / DispWidth;

	if (ratio != 100) {
		w = w * 100 / ratio;
		h = h * 100 / ratio;
		btn_w = btn_w * 100 / ratio;
		btn_h = btn_h * 100 / ratio;
		x = (DispWidth - w) / 2;
		y = (DispHeight - h) / 2;
		htop = htop * 100 / ratio;
		hbottom = hbottom * 100 / ratio;

		ratio += (100 - ratio) / 2;
		font = font * 100 / ratio;
		if (font > 31)
			font = 31;
		if (font < 16)
			font = 16;
		if (font == 17 || font == 19)
			font = 18;
	}

	do {
		Display_Start(phost);

		/// fade the whole LCD
		EVE_Cmd_wr32(phost, COLOR_RGB(0x3F, 0x3F, 0x3F));
		EVE_Cmd_wr32(phost, BEGIN(RECTS));
		EVE_Cmd_wr32(phost, VERTEX2F(0, 0));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(DispWidth), VP(DispHeight)));

		/// diag border
		EVE_Cmd_wr32(phost, COLOR_RGB(0xE1, 0xE1, 0xE1));
		EVE_Cmd_wr32(phost, BEGIN(RECTS));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x), VP(y)));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w), VP(y + h)));

		/// center part
		EVE_Cmd_wr32(phost, COLOR_RGB(0xFF, 0xFF, 0xFF));
		EVE_Cmd_wr32(phost, BEGIN(RECTS));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x + border), VP(y + border)));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w - border), VP(y + h - border)));

		/// top part
		EVE_Cmd_wr32(phost, COLOR_RGB(0x46, 0x50, 0x52));
		EVE_Cmd_wr32(phost, BEGIN(RECTS));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x + border), VP(y + border)));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w - border), VP(y + htop)));

		/// bottom part
		EVE_Cmd_wr32(phost, COLOR_RGB(0xF3, 0xF3, 0xF3));
		EVE_Cmd_wr32(phost, BEGIN(RECTS));
		EVE_Cmd_wr32(phost,
				VERTEX2F(VP(x + border), VP(y + h - hbottom + border)));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(x + w - border), VP(y + h - border)));

		/// diag message
		EVE_Cmd_wr32(phost, COLOR_RGB(0xFF, 0xFF, 0xFF));
		EVE_CoCmd_text(phost, x + border + 10, y + border + 10, font, 0, title);

		uint32_t font2 = font * 8 / 10;
		if (font2 < 16)
			font2 = 16;
		if (font2 == 17 || font2 == 19)
			font2 = 18;
		EVE_Cmd_wr32(phost, COLOR_RGB(0x78, 0x78, 0x78));
		EVE_CoCmd_fillWidth(phost, w);
		EVE_CoCmd_text(phost, x + border + 30, y + h / 2 - 20, font2, OPT_FILL, msg);

		/// diag button yes/no
		EVE_Cmd_wr32(phost, COLOR_RGB(0xFF, 0xFF, 0xFF));
		const uint16_t btn_margin = 25;
		EVE_Cmd_wr32(phost, TAG(tag_y));
		EVE_CoCmd_button(phost, x + w / 2 - btn_w - btn_margin,
				y + h - hbottom + border + (hbottom - btn_h) / 2, btn_w, btn_h,
				font, 0, "Yes");

		EVE_Cmd_wr32(phost, TAG(tag_n));
		EVE_CoCmd_button(phost, x + w / 2 + btn_margin,
				y + h - hbottom + border + (hbottom - btn_h) / 2, btn_w, btn_h,
				font, 0, "No");

		Display_End(phost);

		Gesture_Renew(phost);
		if (Gesture_Get()->tagReleased == tag_y) {
			return true;
		} else if (Gesture_Get()->tagReleased == tag_n) {
			return false;
		}
	} while (true);

	return false;
}

/* This image contains cells: 32x32
Cell 0: Home
Cell 1: Star
Cell 2: Down triangle
Cell 3: Up triangle
Cell 4: Right triangle (Play button icon)
Size: 460 bytes */
PROGMEM prog_uchar8_t home_start_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};

void WelcomeScreen(Gpu_Hal_Context_t *phost, char* info[])
{
	Gpu_Hal_WrCmd32(phost, CMD_INFLATE);
	Gpu_Hal_WrCmd32(phost, 0);
#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
	EVE_Cmd_wrMem(phost,home_start_icon,sizeof(home_start_icon)); //Load from RAM
#else
    EVE_Cmd_wrProgMem(phost,home_start_icon,sizeof(home_start_icon)); //Load from Flash
#endif
    EVE_Cmd_waitFlush(phost);

	Display_Start(phost);
	EVE_Cmd_wr32(phost, BITMAP_HANDLE(0));    // handle for background stars
	EVE_Cmd_wr32(phost, BITMAP_SOURCE(0));      // Starting address in gram
	EVE_Cmd_wr32(phost, BITMAP_LAYOUT(L4, 16, 32));  // format
	EVE_Cmd_wr32(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, 32, 32  ));
	Display_End(phost);

	do
	{
		Display_StartColor(phost, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 219, 180, 150 });
		EVE_Cmd_wr32(phost, SAVE_CONTEXT());    
		EVE_Cmd_wr32(phost, COLOR_A(220));
		EVE_Cmd_wr32(phost, BEGIN(EDGE_STRIP_A));
		EVE_Cmd_wr32(phost, VERTEX2F(0, VP(DispHeight)));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(DispWidth), VP(DispHeight)));
		EVE_Cmd_wr32(phost, COLOR_A(255));
		EVE_Cmd_wr32(phost, RESTORE_CONTEXT());    
		EVE_Cmd_wr32(phost, COLOR_RGB(0, 0, 0));
		
		// information
		EVE_CoCmd_text(phost, DispWidth/2, 20, 28, OPT_CENTER , info[0]);
		EVE_CoCmd_text(phost, DispWidth/2, 60, 26, OPT_CENTER , info[1]);
		EVE_CoCmd_text(phost, DispWidth/2, 90, 26, OPT_CENTER , info[2]);  
		EVE_CoCmd_text(phost, DispWidth/2, 120, 26, OPT_CENTER, info[3]);  
		EVE_CoCmd_text(phost, DispWidth/2, 180, 26, OPT_CENTER, "Click to play");
		
		// play button circle boundary
		EVE_Cmd_wr32(phost, COLOR_RGB(100, 100, 100));
		EVE_Cmd_wr32(phost, BEGIN(FTPOINTS));   
		EVE_Cmd_wr32(phost, POINT_SIZE(20*16));
		EVE_Cmd_wr32(phost, TAG('P'));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(DispWidth/2), VP(150)));
		EVE_Cmd_wr32(phost, COLOR_RGB(180, 35, 35));
		
		// play button
		EVE_Cmd_wr32(phost, BEGIN(BITMAPS));
		EVE_Cmd_wr32(phost, CELL(4));
		EVE_Cmd_wr32(phost, VERTEX2F(VP(DispWidth/2 -14), VP(135)));
		EVE_Cmd_wr32(phost, END());
		Display_End(phost);

	}while(Gpu_Hal_Rd8(phost, REG_TOUCH_TAG)!='P');
	Play_Sound(phost, 0x50, 255, 0xc0);
}
