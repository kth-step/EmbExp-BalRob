/*
 * hw.h
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#ifndef DEV_HW_H_
#define DEV_HW_H_


#include <stdint.h>


void hw_clock_init();

void hw_gpio_init();

void hw_gpio_set( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal );
uint32_t hw_gpio_get( uint32_t portNum, uint32_t bitPosi );
void hw_gpio_set_dir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );


#endif /* DEV_HW_H_ */
