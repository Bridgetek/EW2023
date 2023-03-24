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

#define PAGE_ANGLE 180

#define plugin_left  (195)
#define plugin_right (196)

extern int lastAngle;
extern int step;

static int saveX1;
static int saveX2;

int PP_Init() {
	static int initX = 0;
	Img_t *authIm[] = { &appImages[plugin_left], &appImages[plugin_right]};
	if (initX == 0) {
		for (int i = 0; i < 2; i++) {
			authIm[i]->isFlash = 0;
			authIm[i]->x += DispWidth / 2;
			authIm[i]->y = DispHeight / 2 - authIm[i]->h / 2;
		}
		initX = 1;
		saveX1 = authIm[0]->x;
		saveX2 = authIm[1]->x;
	}
}

int PP_DeInit() {
}


static int progress(E_PAGE param) {
	Img_t *authIm[] = { &appImages[plugin_left], &appImages[plugin_right] };
	static int space = 0;
	static int step = 1;
	static int loop = 0;

	authIm[0]->x = saveX1 - space;
	authIm[1]->x = saveX2 + space;

	Image_Draw(phost, authIm[0]);
	Image_Draw(phost, authIm[1]);

	loop++;
	if (loop > 2) {
		space += step;
		if (space > 5) {
			step = -1;
		}else if (space == 0) {
			step = 1;
		}

		loop = 0;
	}
}

int PP_Draw(E_PAGE param) {
	Display_Start(phost);

	Image_Setup_Rotate(lastAngle, IMG_PAGES.w / 2, IMG_PAGES.h / 2);
	Image_Draw(phost, &IMG_PAGES);
	Image_Draw(phost, &IMG_CAR);

	if (lastAngle == PAGE_ANGLE) {
		progress(param);
	}
	Display_End(phost);

	if (lastAngle != PAGE_ANGLE) {
		lastAngle += step;
		if (lastAngle > 360) {
			lastAngle = 0;
		}
		if (lastAngle < 0) {
			lastAngle = 360;
		}
	}
}

