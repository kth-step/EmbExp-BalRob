/*
 * uart.h
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#ifndef DEV_UART_H_
#define DEV_UART_H_

#include <stdint.h>

void uart_init();
uint8_t uart_read_ready();

#endif /* DEV_UART_H_ */
