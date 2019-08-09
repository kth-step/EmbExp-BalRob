/*
 * timer.c
 *
 *  Created on: 8 aug. 2019
 *      Author: andreas
 */

#include "LPC11xx.h"
#include "dev/hw.h"

#include <stdint.h>


#define TMR_PCLK (12 * 1000 * 1000)
void timer_init() {
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 9);
	LPC_TMR32B0->PR = 0; // no prescaler

	LPC_TMR32B0->TCR = 1;

	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8);
	LPC_TMR16B1->PR = (TMR_PCLK / (4 * 1000)) - 1; // no prescaler

	LPC_TMR16B1->TCR = 1;
}

void timer_start() {
	LPC_TMR32B0->TCR = 2;
	LPC_TMR32B0->TCR = 1;
}


uint32_t timer_read() {
	return LPC_TMR32B0->TC;
}


// use different timer here so that it can be combined together
void timer_wait(uint32_t t) {
	LPC_TMR16B1->TCR = 2;
	LPC_TMR16B1->TCR = 1;
	while (LPC_TMR16B1->TC < t);
}
