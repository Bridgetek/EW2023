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

static float x = 1.5;
static float timeFraction = 0.001;
static char strNo = 0;

int PT_Draw(E_PAGE param) {
	const char str1[] = " EV Charge Point Demo ";
	const char str2[] = " Bridgetek Embedded World 2020 ";
	char s[sizeof(str2)];
	static char lasttam = 0;

	char *str[] = {str1, str2};
	char len[] = {sizeof(str1), sizeof(str2)};

	const length = 400;
	const float speed = 0.002;

	Display_Start(phost);
	App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255)); /// black text on white bg

	// Gpu_CoCmd_GradientA(phost, 0, 0, 0x8080bfff, DispWidth * 2 / 3, DispHeight, 0xffe6f2ff);
	Image_Draw(phost, &appImages[228]);
	Image_Draw(phost, &appImages[229]);

	timeFraction += speed;

	int t = length * (float)((x + 1) * timeFraction - x) * Math_Power(timeFraction, 2);

	if (t < 0) {
		t *= -1;
		lasttam = 1;
	}
	else if (lasttam ==  1){
		strNo = 1;
		lasttam = 0;
	}

	/// End of string
	if (t >= len[strNo]) {
		t = len[strNo] - 1;

		/// End of animation
		if (strNo == 1) {
			timeFraction = 0.001;
			strNo = 0;
			EVE_sleep(1000);
			return 1;
		}
	}

	memset(s, 0, sizeof(s));
	if (t > len[strNo]) {
		t = len[strNo];
	}

	memcpy(s, str[strNo], t);
	Gpu_CoCmd_Text(phost, DispWidth / 2, DispHeight / 2, 30, OPT_CENTERX | OPT_CENTERY, s);

	Display_End(phost);

	return 0;
}

int PT_DeInit() {
	APP_DBG("%s", __FUNCTION__);

}

int PT_Init() {
	strNo = 0;
	timeFraction = 0.001;
}

