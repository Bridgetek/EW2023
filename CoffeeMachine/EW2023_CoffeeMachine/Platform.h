/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_
/* Standard C libraries */
#include <stdio.h>

/* Standard Arduino libraries */
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>

#include "vm801p.h"
#include "printf.h"

#define TRUE		(1)
#define FALSE		(0)

// FT_DataTypes.h
typedef byte             ft_uint8_t;
typedef char             ft_char8_t;
typedef signed char      ft_schar8_t;
typedef unsigned char    ft_uchar8_t;
typedef int              ft_int16_t;
typedef word             ft_uint16_t;
typedef unsigned long    ft_uint32_t;
typedef long             ft_int32_t;
typedef void             ft_void_t;
#define prog_uchar8_t  ft_uchar8_t
#define prog_char8_t   ft_char8_t
#define prog_uint16_t  ft_uint16_t
#define ft_prog_uchar8_t  ft_uchar8_t
#define ft_prog_char8_t   ft_char8_t
#define ft_prog_uint16_t  ft_uint16_t
typedef ft_prog_uchar8_t prog_uchar;
typedef ft_prog_char8_t  prog_char;
typedef ft_prog_uint16_t prog_uint16_t;
typedef prog_uchar8_t    prog_uchar;
typedef prog_char8_t     prog_char;
typedef prog_uint16_t    prog_uint16_t;

typedef char		     bool_t;
typedef char		     char8_t;
typedef unsigned char	 uchar8_t;
typedef signed char	     schar8_t;
typedef float		     float_t;

/* EVE inclusions */
#include "Gpu_Hal.h"
#include "Gpu.h"
#include "CoPro_Cmds.h"
#include "Hal_Utils.h"

#endif /*_PLATFORM_H_*/
