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
#include <dev/timer.h>
#include <dev/imu.h>

#include <stdio.h>


int main(void) {
	hw_clock_init();
	hw_gpio_init();

	ui_init();
	uart_init();
	motor_init();
	timer_init();

	printf("\r\n");
	printf("--------------------------------\r\n");
	printf("Helloooooo there %d", 44);
	printf("\r\n");
	printf("--------------------------------\r\n");

	/*
	if (motor_get_status()) {
		printf("motor fault!\r\n");
	} else {
		printf("motor ok.\r\n");
	}
	*/

	int imu_init_result = 55;
	for (int i = 0; i < 3; i++) {
		imu_init_result = imu_init();
		if (!imu_init_result)
			break;
	}
	if (imu_init_result) {
		printf("imu stuck!!!\r\n");
		while (1);
	}
	printf("--------------------------------\r\n");

    while(1) {

    	// prepare user input
        int i;
    	printf("input: ");
    	fflush(stdout);

    	// wait until a byte arrives
    	while (!uart_read_ready()) {
            char b = ui_get_button();
            ui_set_led(0, b);
            ui_set_led(1, b);

            imu_read_values();
            timer_start();
            imu_wait_new_data();
            uint32_t res_val = timer_read();
            //timer_start();
            //if (res_val < 840000 || res_val > 900000)
            //printf("imu read took: %lu * 10ns\r\n",res_val);
            printf("%d\t%d\t%d\r\n", imu_values[0], imu_values[1], imu_values[2]);
    	}

    	// handle one line
        scanf("%d", &i);
        printf("hello %d done.\r\n", i);

        //timer_start();
        //timer_wait(10*100*1000*1000);
        //uint32_t res_val = timer_read();
        //printf("puh:%d\r\n",res_val);

        //motor_set_f(i/((float)6000),0);

        /*
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
		*/
    }

    return 0;
}
