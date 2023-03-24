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

#define PAGE_ANGLE 90
extern int lastAngle;
extern int step;

static int saveX1;
static int saveX2;

#define IMG_PIN_CELL   (190)
#define IMG_PIN_EMPTY  (191)
#define IMG_PIN_LINE   (192)
#define IMG_PIN_LIGHT  (193)
#define IMG_PIN_SOURCE (194)

int PC_Init() {
	static int initX = 0;
	Img_t *authIm[] = { &appImages[IMG_PIN_CELL], &appImages[IMG_PIN_EMPTY], &appImages[IMG_PIN_LINE], &appImages[IMG_PIN_LIGHT], &appImages[IMG_PIN_SOURCE] };
	if (initX == 0) {
		for (int i = 0; i < 5; i++) {
			authIm[i]->isFlash = 0;
			authIm[i]->x += DispWidth / 2;
			authIm[i]->y += DispHeight / 2 - appImages[IMG_PIN_EMPTY].h / 2 ;
		}
		initX = 1;
		saveX1 = appImages[IMG_PIN_LIGHT].x;
		saveX2 = appImages[IMG_PIN_CELL].x;
	}
}

int PC_DeInit() {
}


static int progress(E_PAGE param) {
	Img_t *authIm[] = { &appImages[IMG_PIN_LINE], &appImages[IMG_PIN_EMPTY], &appImages[IMG_PIN_SOURCE], &appImages[IMG_PIN_LIGHT] };
	static int space = 157;
	static int step = 1;
	static int loop = 0;

	if(param > 100 || param < 0){
		param = 0;
	}

	int num = param / 20;

	appImages[IMG_PIN_LIGHT].x = saveX1 + space;

	for (int i = 0; i < 4; i++) {
		Image_Draw(phost, authIm[i]);
	}

	if (num > 0 && num <= 5) {
		for (int i = 0; i < num; i++) {
			appImages[IMG_PIN_CELL].x = saveX2 + i * 23;
			Image_Draw(phost, &appImages[IMG_PIN_CELL]);
		}
	}

	APP_DBG("pin: %d", param);
	//EVE_sleep(1000);
	
	loop++;
	if (loop > 0) {
		space -= step;
		if (space == 0) {
			space = 157;
		}

		loop = 0;
	}
}

int PC_Draw(E_PAGE param) {
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

