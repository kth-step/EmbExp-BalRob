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

#include <dev/hw.h>
#include <dev/ui.h>
#include <dev/motor.h>
#include <dev/encoder.h>
#include <dev/timer.h>
#include <dev/imu.h>

#include <io.h>
#include <pid.h>


int main(void) {
	hw_clock_init();
	hw_gpio_init();

	ui_init();
	io_init();
	out_info("");
	out_info("--------------------------------");
	out_info("io ready!");

	motor_init();
	encoder_init();
	timer_init();
	out_info("motors, encoders and timers ready!");

#ifndef BOT_BALPEN
	int imu_init_result = 55;
	for (int i = 0; i < 3; i++) {
		imu_init_result = imu_init(1); // enable interrupt handling
		if (!imu_init_result) {
			out_info("imu init done.");
			break;
		}
	}
	if (imu_init_result) {
		out_error("imu stuck!");
		while (1);
	}
#endif

	out_info("startup done!");
	out_info("--------------------------------");

	//pid();

    while(1) {
        out_info_inthex("x", (uint32_t)(encoder_values[0]));
	motor_set_f(-0.7f, 0);
    }

    while(1) {
		char b = ui_get_button();
		ui_set_led(0, b);
		ui_set_led(1, b);
    }

    return 0;
}
