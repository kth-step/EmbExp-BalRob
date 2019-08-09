/*
 * timer.h
 *
 *  Created on: 8 aug. 2019
 *      Author: andreas
 */

#ifndef DEV_TIMER_H_
#define DEV_TIMER_H_

#include <stdint.h>

void timer_init();

void timer_start();

uint32_t timer_read();

// after reading, a timer value can be converted
#define TIMER_TO_10NS(t) (TMR12M_TO_10NS(t))
#define TIMER_TO_US(t) (TIMER_TO_10NS(t)/100)

void timer_wait(uint32_t t);
#define TMR12M_TO_10NS(t) (t * 25 / 3)
#define TMR4K_FROM_10NS(t) (t / 25000)

// use these macros for waiting
#define TIMER_WAIT_10NS(t) (timer_wait(TMR4K_FROM_10NS(t) + 1))
#define TIMER_WAIT_US(t) (TIMER_WAIT_10NS(t*100))

#endif /* DEV_TIMER_H_ */
