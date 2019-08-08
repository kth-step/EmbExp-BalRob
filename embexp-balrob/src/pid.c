/*
 * pid.c
 *
 *  Created on: 8 aug. 2019
 *      Author: andreas
 */

#include <dev/ui.h>
#include <dev/motor.h>
#include <dev/timer.h>
#include <dev/imu.h>

#include <io.h>

#include <stdint.h>
#include <math.h>



#define RAD_TO_DEG		57.29578

const float sampleTime = 0.0033;
const float alpha = 0.9934;

volatile float previousAngle = 0;
volatile float errorPrev = 0;
volatile float errorSum = 0;

volatile float accAngle = 0;
volatile float gyrAngle = 0;


// -------------------------------------------------------------------------------------

float kp = 500.0;
float ki = 10.0;
float kd = 1.0;

volatile float motor_on = 0;

volatile uint32_t pid_handlertime;
volatile uint32_t pid_sampletime;

void pid() {
	while (1) {
		//printf_new("time handler: %i\r\n", pid_handlertime / 100);
		//printf_new("time sample: %i\r\n", pid_sampletime / 100);

		//debug
        //printf_new("\faccX: %d\taccZ: %d\tgyrY: %d       \r", imu_values[0], imu_values[2], imu_values[5]);
        //timer_wait_us(100000);
		printf_new("angle: %f\r\n", previousAngle);
		//printf_new("acc: %f, gyr: %f\r\n", accAngle, gyrAngle);
		//printf_new("accgyrDiff: %f\r\n", accAngle - gyrAngle);
	}
}

#include "LPC11xx.h"
void imu_handler() {
	pid_sampletime = timer_read();
	timer_start();
	imu_read_values();

	// now do some handling
    int16_t accX = imu_values[0];
    int16_t accZ = imu_values[2];
    int16_t gyrY = imu_values[5];

	// calc angle
	accAngle =  (accZ == 0) ? 0 : (atan2(accX,accZ) * RAD_TO_DEG);
	float gyroSpeed = -((((int32_t)gyrY)  * 390) / 32768.0);
	float gyrAngleDiff = gyroSpeed * sampleTime;
	//gyrAngle = gyrAngle + gyrAngleDiff;
	float currentAngle = (alpha * (previousAngle + gyrAngleDiff)) + ((1-alpha) * accAngle);
	previousAngle = currentAngle;

	// maintain error values
	float error = currentAngle - (-15);
	float errorDiff = error - errorPrev;
	float errorSumNew = errorSum + (error);
	errorSum = errorSumNew > 300 ? 300 : (errorSumNew < -300 ? -300 : errorSumNew);

	// apply pid
	float motorPower = kp * error + ki * errorSum + kd * errorDiff;

	// output to motor
	if (motor_on)
		motor_set_f(motorPower, motorPower);

	// finalize the handler with measurements
	pid_handlertime = timer_read();
	//printf_new("time: %i\r\n", pid_handlertime);
}
