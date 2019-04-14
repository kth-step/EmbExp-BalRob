/*
===============================================================================
 Name        : embexp-balrob.c
 Author      : Andreas Lindner
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC11xx.h"

#include <cr_section_macros.h>

#include <dev/hw.h>
#include <dev/uart.h>
#include <dev/ui.h>
#include <dev/motor.h>
#include <dev/imu.h>

#include <stdio.h>


int main(void) {
	hw_clock_init();
	hw_gpio_init();

	ui_init();
	uart_init();
	motor_init();

	printf("\r\n");
	printf("--------------------------------\r\n");
	printf("Helloooooo there %d", 44);
	printf("\r\n");
	printf("--------------------------------\r\n");

	if (motor_get_status()) {
		printf("motor fault!\r\n");
	} else {
		printf("motor ok.\r\n");
	}

	init_imu();

    while(1) {
        /*
        char b = ui_get_button();
        ui_set_led(0, b);
        ui_set_led(1, b);
        */

        int i;
    	printf("input: ");
    	fflush(stdout);
        scanf("%d", &i);
        switch (i) {

        case 1:
        	ui_set_led(0,1);
        	break;
        case 2:
        	ui_set_led(1,1);
        	break;

        case 3:
        	motor_set(1,0);
        	break;
        case 4:
        	motor_set(-1,0);
        	break;

        case 5:
        	motor_set(0,1);
        	break;
        case 6:
        	motor_set(0,-1);
        	break;

		default:
        	ui_set_led(0,0);
        	ui_set_led(1,0);
        	motor_set(0,0);
        	break;
        }
    }

    return 0;
}
