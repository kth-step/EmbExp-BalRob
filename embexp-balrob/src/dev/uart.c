/*
 * uart.c
 *
 *  Created on: 11 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"

/*
Bluetooth (HC-06)
--------------
TX&RX
- PIO1_6/RXD/CT32B0_MAT0
- PIO1_7/TXD/CT32B0_MAT1
*/

/*
--------------------------------------------------------------------------------------------
 */

void uart_init()
{
	// enable RXD
    LPC_IOCON->PIO1_6 &= ~(2<<4)|(1<<0);
    LPC_IOCON->PIO1_6 |= (2<<4)|(1<<0);
    LPC_IOCON->PIO1_6 &= ~(0x1F);
    LPC_IOCON->PIO1_6 |= (0x1);

    // enable TXD
    LPC_IOCON->PIO1_7 &=~(2<<4)|(1<<0);
    LPC_IOCON->PIO1_7 |=(2<<4)|(1<<0);

    // enable clock for UART peripheral
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);

    // set up line control, 8 data bits + 1 stop bit
    LPC_UART->LCR = (0x3 << 0);

    // FIFO disabled?

    // enable RDA interrupt
	//LPC_UART->IER = 1;
	//NVIC_SetPriority(UART_IRQn, 1);
	//NVIC_EnableIRQ(UART_IRQn);

    // enable UART clocking, clock divider setting, 36MHz, baudrate 19200
    LPC_SYSCON->UARTCLKDIV = 36*1000*1000 / 16 / 9600;

}

void uart_write(char c)
{
	while (!(LPC_UART->LSR & 0x20));
	LPC_UART->THR = c;
}

char uart_read()
{
	while (!(LPC_UART->LSR & 0x1));
	return LPC_UART->RBR;
}


/*
--------------------------------------------------------------------------------------------
 */

#include <unistd.h>

// Function returns number of unwritten bytes if error, otherwise 0 for success
int _write(int iFileHandle, char *pcBuffer, int iLength) {
	if (iFileHandle != STDOUT_FILENO) {
	  return -1;
	}

	for (int i = 0; i < iLength; i++) {
		uart_write(pcBuffer[i]);
	}
	return 0;
}


// Function returns number of characters read, stored in pcBuffer
int _read(int iFileHandle, char *pcBuffer, int iLength) {
	if (iFileHandle != STDIN_FILENO) {
	  return -1;
	}

	char c = uart_read();
	switch (c) {
	case '\n':
	case '\r':
		uart_write('\r');
		uart_write('\n');
		break;
	default:
		uart_write(c);
	}

	pcBuffer[0] = c;

	return 1;
}

