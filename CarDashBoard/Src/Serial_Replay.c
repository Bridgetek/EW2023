/**
 * @file replay.c
 * @brief EVE's widgets
 *
 * @author Tuan Nguyen <tuan.nguyen@brtchip.com>
 *
 * @date 2022
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

#include "Common.h"
#include "Platform.h"
#include "EVE_CoCmd.h"
#include "EW2023_CarDashBoard.h" // type CarInfo
#include <math.h>

// This header file too big for FT9xx
#include "EW2023_CarDashBoard_Race_Data.h" //int32_t race[][5] = //{ timems, gear,  rpm, speed, fuel, temp } 

// FT9XX read that file from SDcard
typedef struct _race_line{
	union {
		unsigned char timems4[4];
		uint32_t timems;
	};
	union {
		unsigned char rpm4[4];
		uint32_t rpm;
	};
	unsigned char gear, speed, fuel, temp;
}race_line;

extern SystemStatus_t SystemStatus;

#ifdef FT9XX_PLATFORM
static FIL fp_data;
int serial_replay_task_init() {
	FRESULT fResult;
	BYTE mode;
	char *filePath = TEST_DIR "EW2023_CarDashBoard_Race_Data.dat";

	mode = FA_READ | FA_OPEN_EXISTING;
	fResult = f_open(&fp_data, filePath, mode);
	if (fResult != FR_OK) {
		printf("Cannot open %s, please check SD card, error: %d\n", filePath,
				fResult);
		return 0;
	}else{
		printf("Opened %s \r\n", filePath);
	}

	return 1;
}

int dat_next(race_line *race){
	FRESULT fResult;
	unsigned char dat[9];
	unsigned int bytesread = 9;
	unsigned int bytescount;

	fResult  = f_read(&fp_data, dat, bytesread, &bytescount);
	if (fResult != FR_OK) {
		printf("Error on f_read, error = %u\n", fResult);
		return 0;
	}

	if (bytesread != bytescount) {
		// printf("Reached EOF, stop\n");
		return 0;
	}

	race->timems = 0;
	race->rpm = 0;
	race->timems4[3] = dat[0];
	race->timems4[2] = dat[1];
	race->timems4[1] = dat[2];
	race->gear = dat[3];
	race->rpm4[1] = dat[4];
	race->rpm4[0] = dat[5];
	race->speed = dat[6];
	race->fuel= dat[7];
	race->temp = dat[8];
	return 1;
}
#endif

void serial_replay_task() {
#ifdef FT9XX_PLATFORM
	static uint32_t timeStartMS = 0;
	uint32_t nowMS = EVE_millis();
	if (timeStartMS == 0) {
		timeStartMS = EVE_millis();
	}
	uint32_t dur = nowMS - timeStartMS;

	race_line race;

	int bytes = dat_next(&race);
	while (race.timems < dur && bytes > 0) {
		bytes = dat_next(&race);
	}

	// new loop
	if(bytes == 0){
		f_lseek(&fp_data, 0);
		return;
	}

	SystemStatus.temperature= race.temp;
	SystemStatus.fuel= race.fuel;
	SystemStatus.speed = race.speed;
	SystemStatus.rpm= race.rpm;
#else
	static uint32_t timeStartMS = 0;
	uint32_t nowMS = EVE_millis();
	if (timeStartMS == 0) {
		timeStartMS = EVE_millis();
	}
	uint32_t dur = nowMS - timeStartMS;

	int id = -1;
	for (int i = 0; i < RACE_NUM - 1; i++) {
		if (race[i][0] > dur) {
			id = i;
			break;
		}
	}
	if (id == -1) {
		timeStartMS = EVE_millis();
		return;
	}

	SystemStatus.temperature= race[id][5];
	SystemStatus.fuel= race[id][4];
	SystemStatus.speed = race[id][3];
	SystemStatus.rpm= race[id][2];
#endif
}
