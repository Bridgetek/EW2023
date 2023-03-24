#ifndef EV_CHARGEPOINT_TRANSPORT_H_
#define EV_CHARGEPOINT_TRANSPORT_H_
#include "Common.h"
#include "Uart.h"
#include "EV_ChargePoint_Protocol.h"


inline char *getPagename(E_PAGE p){
	static char *pages[] = {
			"PAGE_SCREEN_SAVER     ",
			"PAGE_CHOOSE_LANGUAGE  ",
			"PAGE_AUTHENTICATE     ",
			"PAGE_PLUGIN           ",
			"PAGE_TRANSACTION      ",
			"PAGE_CHARGING         ",
			"PAGE_REPORT           ",
			"PAGE_MAX              ",
	};

	if(p <= PAGE_MAX && p >= 0){
		return pages[p - 101];
	}
	else{
		return "Error: Page not found";
	}
}

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
inline int Ev_Send_Page(E_PAGE *p) {
	return AppUart_send(p, 1);
}

inline E_PAGE* Ev_Recv_Page() {
	static E_PAGE ret;
	UART_RECV_T *buff = AppUart_recv();
	memcpy(&ret, buff, sizeof(E_PAGE));
	return &ret;
}
#endif /* platform */

#endif /* EV_CHARGEPOINT_TRANSPORT_H_ */
