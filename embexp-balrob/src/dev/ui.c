/*
 * ui.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include <dev/hw.h>
#include "LPC11xx.h"


/*
Button
--------------
- PIO0_1/CLKOUT/CT32B0_MAT2 (with 220 Ohm to GND)

LED
--------------
extra
- R/PIO0_11/AD0/CT32B0_MAT3 (to 3.3V)
onBoard
- PIO0_7/CTS (to GND)
*/

/*
--------------------------------------------------------------------------------------------
 */

char ui_get_button() {
	return !hw_gpio_get(0,1);
}

void ui_set_led(int i, char on) {
	switch (i) {
	case 0:
		hw_gpio_set(0,11,on?0:1);
		break;
	case 1:
		hw_gpio_set(0,7,on?1:0);
		break;
	default:
		break;
	}
}


void ui_init()
{
	// LED PIO0_11
	LPC_IOCON->R_PIO0_11  &= ~0x07;
	LPC_IOCON->R_PIO0_11  |= 0x01;
	hw_gpio_set_dir(0,11,1);

	// PIO0_7
	hw_gpio_set_dir(0,7,1);

	ui_set_led(0, 0);
	ui_set_led(1, 0);

	// Button PIO0_1
	LPC_IOCON->PIO0_1  &= ~0x1F;
	LPC_IOCON->PIO0_1  |= 0x010;
	hw_gpio_set_dir(0,1,0);
}


