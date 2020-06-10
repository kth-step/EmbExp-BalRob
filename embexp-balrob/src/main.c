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

#include <stdint.h>


// calibration follows here
// https://wired.chillibasket.com/2015/01/calibrating-mpu6050/

int buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

int16_t ax, ay, az,gx, gy, gz;

int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz;
int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;

void meansensors() {
  long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;

  while (i<(buffersize+101)){
    imu_wait_new_data();
    imu_read_values();
    // read raw accel/gyro measurements from device
    ax = imu_values[0];
    ay = imu_values[1];
    az = imu_values[2];
    gx = imu_values[4];
    gy = imu_values[5];
    gz = imu_values[6];
    
    if (i>100 && i<=(buffersize+100)){ //First 100 measures are discarded
      buff_ax=buff_ax+ax;
      buff_ay=buff_ay+ay;
      buff_az=buff_az+az;
      buff_gx=buff_gx+gx;
      buff_gy=buff_gy+gy;
      buff_gz=buff_gz+gz;
    }
    if (i==(buffersize+100)){
      mean_ax=buff_ax/buffersize;
      mean_ay=buff_ay/buffersize;
      mean_az=buff_az/buffersize;
      mean_gx=buff_gx/buffersize;
      mean_gy=buff_gy/buffersize;
      mean_gz=buff_gz/buffersize;
    }
    i++;
    //timer_start();
    //timer_wait(2*1000*100);		//wait 2 ms
  }
}

int16_t abs(int16_t v) {
	return v < 0 ? -v : v;
}

uint8_t set_offset_(int16_t acc_x_off, int16_t acc_y_off, int16_t acc_z_off, int16_t gyro_x_off, int16_t gyro_y_off, int16_t gyro_z_off);

void calibration() {
  ax_offset=-mean_ax/8;
  ay_offset=-mean_ay/8;
  az_offset=(16384-mean_az)/8;

  gx_offset=-mean_gx/4;
  gy_offset=-mean_gy/4;
  gz_offset=-mean_gz/4;
  while (1){
    int ready=0;
/*
    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);

    accelgyro.setXGyroOffset(gx_offset);
    accelgyro.setYGyroOffset(gy_offset);
    accelgyro.setZGyroOffset(gz_offset);
*/
    set_offset_(ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset);

    meansensors();
    //Serial.println("...");
    out_info("...");

    if (abs(mean_ax)<=acel_deadzone) ready++;
    else ax_offset=ax_offset-mean_ax/acel_deadzone;

    if (abs(mean_ay)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-mean_ay/acel_deadzone;

    if (abs(16384-mean_az)<=acel_deadzone) ready++;
    else az_offset=az_offset+(16384-mean_az)/acel_deadzone;

    if (abs(mean_gx)<=giro_deadzone) ready++;
    else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

    if (abs(mean_gy)<=giro_deadzone) ready++;
    else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

    if (abs(mean_gz)<=giro_deadzone) ready++;
    else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);

    if (ready==6) break;
  }
}



int main(void) {
	hw_clock_init();
	hw_gpio_init();

	ui_init();
	io_init();
	out_info("");
	out_info("--------------------------------");
	out_info("io ready!");

/*
	motor_init();
	encoder_init();
*/
	timer_init();
//	out_info("motors, encoders and timers ready!");
	out_info("timers ready!");

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

	out_info("startup done!");
	out_info("--------------------------------");




#define BUFF_SAMPLE_SIZE 		100


	out_info("--------------------------------");
	out_info("initializing offsets!");
	out_info("--------------------------------");
	ax_offset = 0xFFFFFA89;
	ay_offset = 0x0000077C;
	az_offset = 0x00000316;
	gx_offset = 0xFFFFFF68;
	gy_offset = 0x0000004E;
	gz_offset = 0x00000002;
	set_offset_(ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset);

	out_info("--------------------------------");
	out_info("imu value range experiment starts!");

//#define CHECKMEANS
#ifdef CHECKMEANS
	out_info("--------------------------------");
	out_info("initial meaning!");
	out_info("--------------------------------");

	meansensors();

	out_info_inthex("value of ax: ", mean_ax);
	out_info_inthex("value of ay: ", mean_ay);
	out_info_inthex("value of az: ", mean_az);
	out_info_inthex("value of gx: ", mean_gx);
	out_info_inthex("value of gy: ", mean_gy);
	out_info_inthex("value of gz: ", mean_gz);
#endif // ifdef CHECKMEANS


#ifndef CHECKMEANS
	while(1) {
		imu_wait_new_data();
		imu_read_values();

		ax = imu_values[0];
		ay = imu_values[1];
		az = imu_values[2];
		gx = imu_values[4];
		gy = imu_values[5];
		gz = imu_values[6];

		out_info_inthex6_16(ax, ay, az, gx, gy, gz);

		timer_start();
		TIMER_WAIT_US(1000 * 1000 / 2);
	}
#endif // ifdef CHECKMEANS



	out_info("--------------------------------");
	out_info("stop!");
	out_info("--------------------------------");
	while (1);

	//pid();

/*
    while(1) {
        out_info_inthex("x", (uint32_t)(encoder_values[0]));
	//motor_set_f(1.0f, 0);
    }
*/

    while(1) {
		char b = ui_get_button();
		ui_set_led(0, b);
		ui_set_led(1, b);
    }

    return 0;
}
