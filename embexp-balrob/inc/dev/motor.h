/*
 * motor.h
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#ifndef DEV_MOTOR_H_
#define DEV_MOTOR_H_

void motor_init();

// status ok = 0, fault = 1
char motor_get_status();

// robot front is towards gyro
// l is left, r is right
// + is to go forward, - is to go backward
//void motor_set(int l, int r);
void motor_set_f(float l, float r);

#endif /* DEV_MOTOR_H_ */
