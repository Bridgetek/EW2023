﻿/*
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

static float x = 1.5;
float timeFraction = 0.001;

int PICB_Draw(E_PAGE param) {
	const float speed = 0.005;
	Display_Start(phost);

	timeFraction += speed;

	int t = 2000.0 * (float)Math_Power(timeFraction, 2) * (float)((x + 1) * timeFraction - x);

	//APP_DBG("x: %f, time: %f, t: %d, d: %f, f: %f", x, timeFraction, t, d, f);

	IMG_CAR.x = t + 20;

	Image_Draw(phost, &IMG_CAR);

	if (t >= DispWidth + 200) {
		timeFraction = 0;
		return 1;
	}

	Display_End(phost);

	return 0;
}

int PICB_DeInit() {
	APP_DBG("%s", __FUNCTION__);
	IMG_CAR.x = 5;
}

int PICB_Init() {
}

