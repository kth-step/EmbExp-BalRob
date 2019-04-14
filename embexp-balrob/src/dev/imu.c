/*
 * imu.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"

/*
IMU (MPU6050)
--------------
SCL&SDA
- PIO0_4/SCL
- PIO0_5/SDA
AD0&INT
- PIO1_10/AD6/CT16B1_MAT1
- PIO1_11/AD7
 */



/*
--------------------------------------------------------------------------------------------
 */


void init_imu() {
	// init AD0, pull down resistor
	LPC_IOCON->PIO1_10  &= ~0x1F;
	LPC_IOCON->PIO1_10  |= 0x08;
}
