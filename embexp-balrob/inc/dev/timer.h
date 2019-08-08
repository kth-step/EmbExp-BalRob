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

// unit is 10 nanoseconds
uint32_t timer_read();
void timer_wait(uint32_t t);

#endif /* DEV_TIMER_H_ */
