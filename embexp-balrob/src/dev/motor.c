/*
 * motor.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"
#include "dev/hw.h"

/*
Motor (DRV8833)
--------------
nFAULT
- R/PIO1_0/AD1/CT32B1_CAP0
IN1-2
- PIO0_8/MISO0/CT16B0_MAT0
- PIO0_9/MOSI0/CT16B0_MAT1
IN3-4
- R/PIO1_1/AD2/CT32B1_MAT0
- R/PIO1_2/AD3/CT32B1_MAT1
 */



/*
--------------------------------------------------------------------------------------------
 */


void motor_init()
{
	//PIO1_0
	LPC_IOCON->R_PIO1_0  &= ~0x07;
	LPC_IOCON->R_PIO1_0  |= 0x01;
	hw_gpio_set_dir(1,0,0);

	//PIO1_1
	LPC_IOCON->R_PIO1_1  &= ~0x07;
	LPC_IOCON->R_PIO1_1  |= 0x01;
	hw_gpio_set_dir(1,1,1);
	hw_gpio_set(1,1,0);
	//PIO1_2
	LPC_IOCON->R_PIO1_2  &= ~0x07;
	LPC_IOCON->R_PIO1_2  |= 0x01;
	hw_gpio_set_dir(1,2,1);
	hw_gpio_set(1,2,0);

	//PIO0_8
	hw_gpio_set_dir(0,8,1);
	hw_gpio_set(0,8,0);
	//PIO0_9
	hw_gpio_set_dir(0,9,1);
	hw_gpio_set(0,9,0);

	// initialize timers and setup for PWM
}

char motor_get_status() {
	return !hw_gpio_get(1,0);
}


void motor_set(int l, int r) {
	if (l < 0) {
		hw_gpio_set(1,1,1);
		hw_gpio_set(1,2,0);
	} else if (l == 0) {
		hw_gpio_set(1,1,0);
		hw_gpio_set(1,2,0);
	} else {
		hw_gpio_set(1,1,0);
		hw_gpio_set(1,2,1);
	}

	if (r < 0) {
		hw_gpio_set(0,8,0);
		hw_gpio_set(0,9,1);
	} else if (r == 0) {
		hw_gpio_set(0,8,0);
		hw_gpio_set(0,9,0);
	} else {
		hw_gpio_set(0,8,1);
		hw_gpio_set(0,9,0);
	}
}
