#ifndef UART_H_
#define UART_H_
#include "Common.h"
#include "EVE_CoCmd.h"

#if defined(FT9XX_PLATFORM) || defined(RP2040_PLATFORM)
#define UART_BUFFER_SIZE (16)
typedef struct _UART_RECV_T{
	uint8_t     data[UART_BUFFER_SIZE];
    uint16_t    size;
} UART_RECV_T;

int AppUart_setup();
int AppUart_Enable_Uart1();
UART_RECV_T *AppUart_recv();
int AppUart_send(char *buffer, int size);

#endif /* platform */
#endif /* UART_H_ */
