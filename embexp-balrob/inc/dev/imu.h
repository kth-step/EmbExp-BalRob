/*
 * imu.h
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#ifndef DEV_IMU_H_
#define DEV_IMU_H_

#include <stdint.h>

extern volatile int16_t imu_values[7];

uint8_t imu_init();
void imu_wait_new_data();
void imu_read_values();

#endif /* DEV_IMU_H_ */
