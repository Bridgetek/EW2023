#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
#define TEST_DIR                            "..\\..\\Test\\"
#else
#define TEST_DIR                            "/Test/BarTypeLCD"
#endif

#define ENABLE_SERVER_MODE 0

#include "Common.h"

#define UI1_ENABLE                          0
#define UI2_ENABLE                          1
				                            
#define IMG_CAR                             (appImages[197])
#if UI1_ENABLE	                            
#define appImages                           appImages_1
#define IMG_PAGES                           (appImages[198])
#elif UI2_ENABLE                            
#define appImages                           appImages_1
#define IMG_PAGES                           (appImages[0])
#endif	
extern Img_t appImages[];
extern EVE_HalContext* phost;

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
