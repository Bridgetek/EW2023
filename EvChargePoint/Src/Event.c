﻿#include "Event.h"
#include <time.h>
#define MIN_MOVE         15
#define MIN_TOUCH        14

void getTouchEvent(Gpu_Hal_Context_t *phost, int *x, int *y, int8_t *isTouch)
{
	*x = Gpu_Hal_Rd16(phost, REG_TOUCH_SCREEN_XY);
	*y = Gpu_Hal_Rd16(phost, REG_TOUCH_SCREEN_XY + 2);

	*isTouch = !(Gpu_Hal_Rd16(phost, REG_TOUCH_RAW_XY) & 0x8000);
}

uchar8_t is_touch(Gpu_Hal_Context_t *phost) {
	uchar8_t ret = !(Gpu_Hal_Rd16(phost, REG_TOUCH_RAW_XY) & 0x8000);
	return ret;
}
