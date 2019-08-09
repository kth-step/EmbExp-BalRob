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




// -------------------------------------------------------------------------------------
// message interface
// -------------------------------------------------------------------------------------


typedef struct pid_msg
{
	uint32_t pid_sampletime;
	uint32_t pid_handlertime;
	uint32_t pid_counter;

	float angle;
	float error;
	float errorDiff;
	float errorSum;

	uint8_t last_noyield;
} pid_msg_t;

volatile uint8_t msg_flag = 0;
volatile pid_msg_t pid_msg_last;

uint8_t pid_msg_write(pid_msg_t m) {
	if (msg_flag)
		return 1;

	pid_msg_last = m;
	msg_flag = 1;
	return 0;
}
uint8_t pid_msg_read(pid_msg_t* m) {
	if (!msg_flag)
		return 1;

	*m = pid_msg_last;
	msg_flag = 0;
	return 0;
}


// -------------------------------------------------------------------------------------
// pid controller in the imu_handler
// -------------------------------------------------------------------------------------

#define RAD_TO_DEG		(57.29578)
#define SAMPLE_TIME		(0.005)
#define ALPHA			(0.9934)

// inputs
volatile uint8_t motor_on = 1;
volatile float angleTarget = -15;
volatile float kp = 500.0;
volatile float ki = 10.0;
volatile float kd = 1.0;

// output
volatile float motorPower = 0;

// controller state
float angleLast = 0;
float errorLast = 0;
float errorSum = 0;
uint32_t pid_counter = 0;

void imu_handler(uint8_t noyield) {
	// start by taking the time since the last run, restarting the timer and reading the imu
	uint32_t pid_sampletime = timer_read();
	timer_start();
	imu_read_values();

	// pick out the relevant imu values
    int16_t accX = imu_values[0];
    int16_t accZ = imu_values[2];
    int16_t gyrY = imu_values[5];

	// calc angle using complementary filter
	float accAngle =  (accZ == 0) ? 0 : (atan2(accX,accZ) * RAD_TO_DEG);
	float gyrAngleDiff = (-((((int32_t)gyrY)  * 390) / 32768.0)) * SAMPLE_TIME;

	float angle = (ALPHA * (angleLast + gyrAngleDiff)) + ((1-ALPHA) * accAngle);
	angleLast = angle;

	// compute error and its derivative and integral
	float error = angle - angleTarget;
	float errorDiff = error - errorLast;
	errorLast = error;
	float errorSumNew = errorSum + (error);
	errorSum = errorSumNew > 300 ? 300 : (errorSumNew < -300 ? -300 : errorSumNew);

	// compute output signal
	float motorPowerNew = (kp * error) + (ki * errorSum * SAMPLE_TIME) + (kd * errorDiff / SAMPLE_TIME);
	motorPower = motorPowerNew;

	// output to motor
	if (motor_on)
		motor_set_f(motorPowerNew, motorPowerNew);
	else
		motor_set_f(0, 0);

	// finalize the handler with counter maintenance and a time measurement
	pid_counter++;
	uint32_t pid_handlertime = timer_read();

	//TIMER_WAIT_US(2300);

	// prepare message
	pid_msg_write((pid_msg_t){ .pid_sampletime = pid_sampletime,
							   .pid_handlertime = pid_handlertime,
							   .pid_counter = pid_counter,

							   .angle = angle,
							   .error = error,
							   .errorDiff = errorDiff,
							   .errorSum = errorSumNew,

							   .last_noyield = noyield});
}


// -------------------------------------------------------------------------------------
// communication loop
// -------------------------------------------------------------------------------------


void pid() {
	pid_msg_t pid_msg;
	while (1) {
		// read the latest pid message
		while (pid_msg_read(&pid_msg));

		if (pid_msg.last_noyield)
			io_debug("last imu handler was too slow.");

		printf_new("time handler: %ius\r\n", TIMER_TO_US(pid_msg.pid_handlertime));
		printf_new("time sample: %ius\r\n", TIMER_TO_US(pid_msg.pid_sampletime));
		printf_new("counter: %i\r\n", pid_msg.pid_counter);

		//debug
        //printf_new("\faccX: %d\taccZ: %d\tgyrY: %d       \r", imu_values[0], imu_values[2], imu_values[5]);
		//printf_new("angle: %f\r\n", angleLast);
	}
}



