#ifndef CALIBRATE_H_
#define CALIBRATE_H_

#include <stdint.h>

#define BUFF_SAMPLE_SIZE 		100

#define ACC_SENSITIVITY			2
#define ACC_1G					16384
#define GYRO_SENSITIVITY		4

#define ACC_X_OFF				-2360
#define ACC_Z_OFF				1076
#define GYRO_Y_OFF				17

#define ACC_X_OFF_H 			0x06
#define ACC_X_OFF_L				0x07
#define ACC_Z_OFF_H				0x0A
#define ACC_Z_OFF_L				0x0B
#define GYRO_Y_OFF_H			0x15
#define GYRO_Y_OFF_L			0x16


/**
 * @brief	Calculate the mean for acc_x, acc_z, gyro_y on BUFF_SAMPLE_SIZE samples
 * @param	acc_x_mean : address to store the result of the mean for acc_x
 * 			acc_z_mean : address to store the result of the mean for acc_z
 * 			gyro_y_mean : address to store the result of the mean for gyro_y
 * @return	Nothing
 * @note	Doesn't use the first 20 samples
 */
void calculate_mean(int16_t *acc_x_mean, int16_t *acc_z_mean, int16_t *gyro_y_mean);

/**
 * @brief	Calculate the offset for acc_x, acc_z, gyro_y
 * @param	acc_x_offset_start : starting value for the offset of acc_x
 * 			acc_z_offset_start : starting value for the offset of acc_z
 * 			gyro_y_offset_start : starting value for the offset of gyro_y
 * @return	Nothing
 * @note	Modify ACC_SENSITIVITY and GYRO_SENSITIVITY to set the precision wanted.
 */
void calculate_offset(int16_t acc_x_offset_start, int16_t acc_z_offset_start, int16_t gyro_y_offset_start);

/**
 * @brief	Set the offset to the offset register of the IMU MPU-6050
 * @param	acc_x_off : value for the offset of acc_x
 * 			acc_z_off : value for the offset of acc_x
 * 			gyro_y_off : value for the offset of gyro_y
 * @return	0 if successful, 1 if timeout in writing into the IMU offset registers
 * @note	Nothing
 */
uint8_t set_offset(int16_t acc_x_off, int16_t acc_z_off, int16_t gyro_y_off);

#endif /* CALIBRATE_H_ */
