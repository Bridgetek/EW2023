#ifndef APP_BARLCD_EW2020_H_
#define APP_BARLCD_EW2020_H_

#include "EV_ChargePoint_Transport.h"

void App_BarLCD_EW2020();

// Function pointer table for common rendering interface (32 bytes)
typedef struct _PAGE {
	union {
		void(*Table[3])(void);
		struct {
			// Pointers to slot functions
			int(*Init)(void);
			int(*Draw)(E_PAGE);
			int(*DeInit)(void);
		};
	};

} AP;

#endif /* APP_BARLCD_EW2020_H_ */
