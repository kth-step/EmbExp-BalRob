/*
 * uart.h
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#ifndef DEV_UART_H_
#define DEV_UART_H_

void uart_init();

// returns -1 if the device is busy
int uart_write(char c);
int uart_read();

#endif /* DEV_UART_H_ */
