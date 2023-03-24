/**
 * @file Eve_Ext_Input_RP2040.c
 * @brief Program's input
 *
 * @author Bridgetek
 *
 * @date Jan 2023
 *
 * Copyright (c) Bridgetek Pte Ltd.
 * Software License Agreement
 *
 * This code is provided as an example only and is not guaranteed by Bridgetek.
 * BRIDGETEK accept no responsibility for any issues resulting from its use.
 * The developer of the final application incorporating any parts of this
 * sample project is responsible for ensuring its safe and correct operation
 * and for any consequences resulting from its use.
 */

#include "Platform.h"
#if defined(RP2040_PLATFORM)

#include "pico/bootrom.h"
#include "hardware/watchdog.h"

// ------ Extension Internal variable --------------------------------------------
// None

// ------ Extension External variable --------------------------------------------
// None

// ------ Extension Internal function --------------------------------------------
static void _EVE_Ext_Input_Check_Reboot(char c){
	if(c == 'u') {
		reset_usb_boot(0, 0);
		exit(0);
	}
	else if(c == 'r') {
		watchdog_enable(1, 1);
	}
}

// ------ Extension External function --------------------------------------------
char EVE_Ext_GetChar(){
	int c = getchar_timeout_us(10);
	_EVE_Ext_Input_Check_Reboot(c);
	if (c == PICO_ERROR_TIMEOUT) {
		c = 0;
	}
	return (char)c;
}
#else

#endif