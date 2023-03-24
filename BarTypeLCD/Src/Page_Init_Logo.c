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
#include "App.h"

static void Draw_Logo_Pieces(int space, int red, int green, int blue) {
	const int imageW = 360;
	const int imageH = 120;
	const int numberW = 18;
	const int numberH = 10;
	const int numberSpaceW = numberW - 1;
	const int numberSpaceH = numberH - 1;
	const int partW = 20;
	const int partH = 12;

	const int imageX = (DispWidth - imageW) / 2;

	int xi, yi;

	Img_t *im;
	Display_Start(phost);
	App_WrCoCmd_Buffer(phost, COLOR_RGB(red, green, blue));

	int iw = numberSpaceW * space + imageW;
	int ih = numberSpaceH * space + imageH;
	int ix = (DispWidth - iw) / 2;
	int iy = (DispHeight - ih) / 2;

	if (iw > DispWidth) {
		ix = -(iw - DispWidth) / 2;
	}
	if (ih > DispHeight) {
		iy = -(ih - DispHeight) / 2;
	}

	for (int i = 0; i < numberW * numberH; i++) {
		int column = i % numberW;
		int row = i / numberW;

		xi = column * (partW + space);
		yi = row * (partH + space);

		xi += ix;
		yi += iy;
		if (xi < DispWidth && (xi + partW) > 0) {
			if (yi < DispHeight && (yi + partH) > 0) {
				im = &appImages[i];
				im->x = xi;
				im->y = yi;

				Image_Draw(phost, im);

			}
		}
	}
	Display_End(phost);
}

static int space = 100;
static int red = 0;
static int isDrawDone = 0;
const int stepSpace = 3;
int PILG_Draw(E_PAGE param) {
	const int stepColor = 8;
	if (space >= 0) {
		Draw_Logo_Pieces(space, 0, 0, 0);
		EVE_sleep(50);

		space -= stepSpace;

		if (space < 0 && space > -stepSpace) {
			space = 0;
		}
		return isDrawDone;
	}

	if (red < 255) {
		Draw_Logo_Pieces(0, red, red, red);
		red += stepColor;
		return isDrawDone;
	}
	isDrawDone = 1;
	return isDrawDone;
}

int PILG_DeInit() {
	APP_DBG("%s", __FUNCTION__);
	if(isDrawDone){
		space = 0;
		while (space < 200) {
			Draw_Logo_Pieces(space, 0, 0, 0);
			EVE_sleep(5);
			space += stepSpace;
		}
	}
}

int PILG_Init() {
	APP_DBG("%s", __FUNCTION__);
	space = 100;
	red = 0;
	isDrawDone = 0;
}

