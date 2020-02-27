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

#include <robot_params.h>

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

#define RAD_TO_DEG		(57.29578f)
#define SAMPLE_TIME		(0.005f)
#define ALPHA			(0.9934f)


// inputs
#ifdef BOT_LEGO
volatile uint8_t motor_on = 1;
#endif
#ifdef BOT_MINI
volatile uint8_t motor_on = 0;
#endif
volatile float angleTarget = ANGLETARGET;
volatile float kp = INIT_KP;
volatile float ki = INIT_KI;
volatile float kd = INIT_KD;

// output
//#define DEBUG_ANGLESCALE
#ifdef DEBUG_ANGLESCALE
volatile float accAngle_ = 0;
volatile float gyrAngle_ = 0;
#endif
volatile float motorPower = 0;

// controller state
float angleLast = 0;
float errorLast = 0;
float errorSum = 0;
uint32_t pid_counter = 0;

#ifdef DONTKNOWMATH

static float abs_own(float f) {
	if (f < 0)
		return -1 * f;
	else
		return f;
}

#define ATAN2F_FUN atan2f_own
static float atan2f_own(float y, float x) {
	float coeff_1 = M_PI / 4.0f;
	float coeff_2 = 3.0f * coeff_1;
	float abs_y = abs_own(y);
	float angle;
	if (x < 0.0f) {
		float r = (x + abs_y) / (abs_y - x);
		angle = coeff_2 - coeff_1 * r;
	} else {
		float r = (x - abs_y) / (x + abs_y);
		angle = coeff_1 - coeff_1 * r;
	}
	return y < 0.0f ? -angle : angle;
}
#else
#define ATAN2F_FUN atan2f
#endif

void imu_handler_pid_entry(uint8_t noyield, uint32_t pid_sampletime);

void imu_handler(uint8_t noyield) {
	// start by taking the time since the last run, restarting the timer and reading the imu
	uint32_t pid_sampletime = timer_read();
	timer_start();
	imu_read_values();

	// continue
	imu_handler_pid_entry(noyield, pid_sampletime);
}

void imu_handler_pid_entry(uint8_t noyield, uint32_t pid_sampletime) {

	// pick out the relevant imu values
    int16_t accX = imu_values[0];
    int16_t accZ = imu_values[2];
    int16_t gyrY = imu_values[5];

	// calc angle using complementary filter
	float accAngle =  (accZ == 0) ? 0 : (ATAN2F_FUN(accX,accZ) * RAD_TO_DEG);
	float gyrAngleDiff = (-((((int32_t)gyrY)  * GYR_SCALE) / 32768.0f)) * SAMPLE_TIME;

#ifdef DEBUG_ANGLESCALE
	accAngle_  = accAngle;
	gyrAngle_ += gyrAngleDiff;
#endif

	float angle = (ALPHA * (angleLast + gyrAngleDiff)) + ((1-ALPHA) * accAngle);
	angleLast = angle;

	// compute error and its derivative and integral
	float error = angle - angleTarget;
	float errorDiff = error - errorLast;
	errorLast = error;
	float errorSumNew = errorSum + (error);
	errorSum = (!(errorSumNew < 300)) ? 300 : (errorSumNew < -300 ? -300 : errorSumNew);

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


uint8_t button_last = 0;
void pid() {
	pid_msg_t pid_msg;

	// turn red led on
	ui_set_led(0, 1);
	ui_set_led(1, 1);

	while (1) {
		int in_ch;

		// handle button, hope that the loop is so slow that it is already debounced
#ifdef BOT_MINI
		uint8_t button = ui_get_button();
		if (button != button_last && button == 0)
			motor_on = !motor_on;
		button_last = button;
#endif

		// handle io
		while ((in_ch = in_handle()) == -3);

		switch (in_ch) {
		case -1:
			// nothing available
			break;
		case -2:
			// start sync error
			break;
		case 50:
			motor_on = in_data;
			break;
		case 60:
			kp = *((float*)&in_data);
			out_info("KP! %iu", pid_msg.pid_counter);
			break;
		case 61:
			ki = *((float*)&in_data);
			out_info("KI! %iu", pid_msg.pid_counter);
			break;
		case 62:
			kd = *((float*)&in_data);
			out_info("KD! %iu", pid_msg.pid_counter);
			break;
		case 70:
			angleTarget = *((float*)&in_data);
			out_info("angletarget! %iu", pid_msg.pid_counter);
			break;
		default:
			if (in_ch >= 0) {
				// unknown channel
			} else {
				// some error
			}
			break;
		}

		// read the latest pid message
		while (pid_msg_read(&pid_msg));

		if (pid_msg.last_noyield)
			out_debug("last imu handler was too slow.");

		out_data(10, (uint8_t*)&pid_msg, sizeof(pid_msg));
		//printf_new("time handler: %ius\r\n", TIMER_TO_US(pid_msg.pid_handlertime));
		//printf_new("time sample: %ius\r\n", TIMER_TO_US(pid_msg.pid_sampletime));
		//printf_new("counter: %i\r\n", pid_msg.pid_counter);

		//debug
#ifdef DEBUG_ANGLESCALE
		out_info("acc: %f\t gyr: %f\r\n", accAngle_, gyrAngle_);
#endif
        //printf_new("\faccX: %d\taccZ: %d\tgyrY: %d       ", imu_values[0], imu_values[2], imu_values[5]);
        //TIMER_WAIT_US(100000);

		/*
		int32_t angle = (int32_t)(angleLast * 1000);
		int32_t angle_h = angle / 1000;
		int32_t angle_l = (angle < 0 ? -angle : angle)%1000;
		out_info("angle: %d.%d", angle_h, angle_l);
		*/
	}
}



