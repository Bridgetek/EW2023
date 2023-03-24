/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
 * RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
 * RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 *
 * Abstract: Application to demonstrate function of EVE.
 * Author : Bridgetek
 *
 * Revision History:
 * 1.0 - date 2018.07.30 - Initial for BT816
 */

#include "Common.h"
#include "Uart.h"

#if defined(FT9XX_PLATFORM)

/* Transmit buffer */
static UART_RECV_T mUartBuffer = { { 0 }, 0, 0 };

/**
 The Interrupt which handles asynchronous transmission and reception
 of data into the ring buffer
 */
void uart0ISR() {
	static uint8_t c;
	static uint16_t avail;

	/* Transmit interrupt... */
	if (uart_is_interrupted(UART0, uart_interrupt_tx)) {
	}

	/* Receive interrupt... */
	if (uart_is_interrupted(UART0, uart_interrupt_rx)) {

		/* Read a byte from the UART... */
		/* This call will block the CPU until a character is received */
		uart_read(UART0, &c);
		//APP_DBG("Get %d, size= %d", c, mUartBuffer.size);

		mUartBuffer.data[mUartBuffer.size] = c;

		/* Increment the pointer and wrap around */
		mUartBuffer.size++;
		if (mUartBuffer.size > UART_BUFFER_SIZE) {
			memset(&mUartBuffer, 0, sizeof(mUartBuffer));
			mUartBuffer.data[0] = c;
			mUartBuffer.size = 1;
		}
	}
}

int AppUart_Enable_Uart1() {
#define GPIO_UART1_TX (52)
#define GPIO_UART1_RX (53)
	sys_enable(sys_device_uart1);
	gpio_function(GPIO_UART1_TX, pad_uart1_txd); /* UART1 TXD */
	gpio_function(GPIO_UART1_RX, pad_uart1_rxd); /* UART1 RXD */
	uart_open(UART1, /* Device */
	1, /* Prescaler = 1 */
	UART_DIVIDER_115200_BAUD, /* Divider = 1302 */
	uart_data_bits_8, /* No. Data Bits */
	uart_parity_none, /* Parity */
	uart_stop_bits_1); /* No. Stop Bits */

	return 1;
}

int AppUart_setup() {
	/* Attach the interrupt so it can be called... */
	interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
	/* Enable the UART to fire interrupts when receiving data... */
	uart_enable_interrupt(UART0, uart_interrupt_rx);
	/* Enable the UART to fire interrupts when transmitting data... */
	uart_enable_interrupt(UART0, uart_interrupt_tx);
	/* Enable interrupts to be fired... */
	uart_enable_interrupts_globally(UART0);
	interrupt_enable_globally();

	return 1;
}

UART_RECV_T *AppUart_recv() {
	static UART_RECV_T ret = { { 0 }, 0, 0 };
	if (mUartBuffer.size == 0) {
		return &mUartBuffer;
	}
	//APP_DBG("Read %d bytes", mUartBuffer.size);

	memcpy(&ret, &mUartBuffer, sizeof(mUartBuffer));
	memset(&mUartBuffer, 0, sizeof(mUartBuffer));

	return &ret;
}

int AppUart_send(char *buffer, int size) {
//	APP_DBG("Write %d bytes, char: %c", size, buffer);
	uart_writen(UART0, buffer, size);
	return 1;
}


#endif /* platform */
