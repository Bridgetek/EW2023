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

#if defined(_WIN32)
#include <conio.h> // _kbhit

 // ------ Extension Internal variable --------------------------------------------
// None

// ------ Extension External variable --------------------------------------------
// None

// ------ Extension Internal function --------------------------------------------
// None

// ------ Extension External function --------------------------------------------
char EVE_Ext_GetChar(){
	if (_kbhit()) {
        char c = _getch();
		return c;
	}
	return 0;
}
#else

#endif