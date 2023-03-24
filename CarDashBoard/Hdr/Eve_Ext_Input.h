/**
 * @file Eve_Ext_Input.h
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
#ifndef _EVE_EXT_INPUT_H__
#define _EVE_EXT_INPUT_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ------ Extension ID and version -----------------------------------------------
#define EVE_EXT_INPUT_ENABLE 1
#define EVE_EXT_INPUT_HEADER_VERSIONING 1
// ------ Extension Internal type ------------------------------------------------
// None

// ------ Extension External type ------------------------------------------------
// None

// ------ Extension External function --------------------------------------------
char EVE_Ext_GetChar();
#endif /* _EVE_EXT_INPUT_H__ */
