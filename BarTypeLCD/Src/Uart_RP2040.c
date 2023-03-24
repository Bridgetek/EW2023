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

#if defined(RP2040_PLATFORM)
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#define UART_ID   uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

/* Transmit buffer */
static UART_RECV_T mUartBuffer = { { 0 }, 0, 0 };

/**
The Interrupt which handles asynchronous transmission and reception
of data into the ring buffer
*/
void on_uart_rx() {
	while (uart_is_readable(UART_ID)) {
		uint8_t ch = uart_getc(UART_ID);

        mUartBuffer.data[mUartBuffer.size] = ch;

        /* Increment the pointer and wrap around */
        mUartBuffer.size++;
        if (mUartBuffer.size > UART_BUFFER_SIZE) {
            memset(&mUartBuffer, 0, sizeof(mUartBuffer));
            mUartBuffer.data[0] = ch;
            mUartBuffer.size = 1;
        }
	}
}

int AppUart_Enable_Uart1() {
	return 1;
}

int AppUart_setup() {
    // Set up our UART with a basic baud rate.
    uart_init(UART_ID, 2400);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
	
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
	uart_write_blocking(UART_ID, (const uint8_t * )buffer, (size_t)size);
	return 1;
}


#endif /* platform */
