#ifndef APP_EW2023_CARSEATADJUSTMENT_H_
#define APP_EW2023_CARSEATADJUSTMENT_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/EW2023_CarSeatAdjustment/Test"
#endif

#define GET_CALIBRATION                     1

#endif /* APP_EW2023_CARSEATADJUSTMENT_H_ */
