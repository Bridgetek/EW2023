/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 */

#ifndef _VM801P_H_
#define _VM801P_H_

#define EVE_2		/* for FT81x series */
#define ARDUINO_PLATFORM
#define ARDUINO_PLATFORM_COCMD_BURST
#define __VM801P__
#define GPIO_CS     9
#define GPIO_PD     4
#define FT81X_ENABLE
#define SD_CSPIN 8

// #define DispWidth      800L
// #define DispHeight     480L
// #define DispHCycle     928L
// #define DispHOffset    88L
// #define DispHSync0     0L
// #define DispHSync1     48L
// #define DispVCycle     525L
// #define DispVOffset    32L
// #define DispVSync0     0L
// #define DispVSync1     3L
// #define DispPCLK       2
// #define DispSwizzle    0
// #define DispPCLKPol    1
// #define DispCSpread    0
// #define DispDither     1

// #define DispWidth      480L
// #define DispHeight     272L
// #define DispHCycle     548L
// #define DispHOffset    43L
// #define DispHSync0     0
// #define DispHSync1     41L
// #define DispVCycle     292L
// #define DispVOffset    12L
// #define DispVSync0     0
// #define DispVSync1     10L
// #define DispPCLK       5L
// #define DispSwizzle    0
// #define DispPCLKPol    1L
// #define DispCSpread    1L
// #define DispDither     1L

#define DispWidth      480L
#define DispHeight     272L
#define DispHCycle     525L
#define DispHOffset    43L
#define DispHSync0     0
#define DispHSync1     41L
#define DispVCycle     286L
#define DispVOffset    12L
#define DispVSync0     0
#define DispVSync1     10L
#define DispPCLK       5L
#define DispSwizzle    0
#define DispPCLKPol    1L
#define DispCSpread    1L
#define DispDither     1L

#include "BT88x.h"

#endif /*_VM801P_H_*/
