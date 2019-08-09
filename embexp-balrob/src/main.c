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
#include <dev/ui.h>
#include <dev/motor.h>
#include <dev/timer.h>
#include <dev/imu.h>

#include <io.h>
#include <pid.h>


int main(void) {
	hw_clock_init();
	hw_gpio_init();

	ui_init();
	io_init();
	io_info("");
	io_info("--------------------------------");
	io_info("io ready!");

	motor_init();
	timer_init();
	io_info("motors and timers ready!");

	int imu_init_result = 55;
	for (int i = 0; i < 3; i++) {
		imu_init_result = imu_init(1); // enable interrupt handling
		if (!imu_init_result) {
			io_info("imu init done.");
			break;
		}
	}
	if (imu_init_result) {
		io_error("imu stuck!");
		while (1);
	}
	io_info("startup done!");
	io_info("--------------------------------");

	pid();

    while(1) {
		char b = ui_get_button();
		ui_set_led(0, b);
		ui_set_led(1, b);
    }

    return 0;
}
