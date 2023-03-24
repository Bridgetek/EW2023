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

#define PAGE_ANGLE 270

Img_t *imagecard = &appImages[185];
Img_t *bluecard = &appImages[180];

extern int lastAngle;
extern int step;

int PA_Init() {
	static int initX = 0;
	Img_t *authIm[] = { bluecard, &appImages[181], &appImages[182], &appImages[183], &appImages[184], imagecard };

	if (initX == 0) {
		for (int i = 0; i < 6; i++) {
			authIm[i]->isFlash = 0;
			authIm[i]->x += DispWidth / 2;
		}
		initX = 1;
	}
}

int PA_DeInit() {
}

static int progress(E_PAGE param) {
	Img_t *authIm[] = {0 , &appImages[181], &appImages[182], &appImages[183], &appImages[184]};
	static int power = 1;
	static int card = 0;
	static int loop = 0;
	
	Image_Draw(phost, imagecard);
	if (power > 0) {
		Image_Draw(phost, authIm[power]);
	}
	if (card) {
		Image_Draw(phost, bluecard);
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
}

int PA_Draw(E_PAGE param) {
	Display_Start(phost);
	
	Image_Setup_Rotate(lastAngle, IMG_PAGES.w / 2, IMG_PAGES.h / 2);
	Image_Draw(phost, &IMG_PAGES);
	Image_Draw(phost, &IMG_CAR);

	progress(param);

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

