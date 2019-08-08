//#define CALIB_ME

// NOTE: I BROKE THIS CODE

#ifdef CALIB_ME

#include <dev/calibrate.h>
#include <dev/i2c.h>
#include <dev/imu.h>
#include <dev/timer.h>
#include <stdlib.h>
#include <io.h>

extern const uint8_t imu_address;

void calculate_mean(int16_t *acc_x_mean, int16_t *acc_z_mean, int16_t *gyro_y_mean)
{
	int16_t i = 0;
	int32_t acc_x_buff = 0, acc_z_buff = 0, gyro_y_buff = 0;

	timer_start();
	while (i < (BUFF_SAMPLE_SIZE + 21)) {
		I2C_Read_Blocking(imu_address, 0x3B, 14);					//we update the values of the IMU

		if ((i > 20) && (i <= (BUFF_SAMPLE_SIZE + 20))) {			//we don't take the first 100 values

			acc_x_buff += imu_values[0];
			acc_z_buff += imu_values[2];
			gyro_y_buff += imu_values[5];
		}

		if (i == (BUFF_SAMPLE_SIZE + 20)) {						//last sample, we compute the mean
			*acc_x_mean = acc_x_buff / BUFF_SAMPLE_SIZE;
			*acc_z_mean = acc_z_buff / BUFF_SAMPLE_SIZE;
			*gyro_y_mean = gyro_y_buff / BUFF_SAMPLE_SIZE;
		}
		++i;
		timer_wait(10*1000*100);		//wait till 10 ms
		timer_start();
	}
}


uint8_t set_offset(int16_t acc_x_off, int16_t acc_z_off, int16_t gyro_y_off);
void calculate_offset(int16_t acc_x_offset_start, int16_t acc_z_offset_start, int16_t gyro_y_offset_start)
{
	int8_t passCounter;

	int16_t acc_x_offset = acc_x_offset_start, acc_z_offset = acc_z_offset_start, gyro_y_offset = gyro_y_offset_start;
	int16_t acc_x_mean, acc_z_mean, gyro_y_mean;

	if (set_offset(acc_x_offset, acc_z_offset, gyro_y_offset)) {	//if we fail to set the offsets, we quit the function but we print that we failed
		io_error("TIMEOUT\n");
		return;
	}

	calculate_mean(&acc_x_mean, &acc_z_mean, &gyro_y_mean);

	acc_x_offset = - acc_x_mean / ACC_SENSITIVITY;			//acc offset values are on LSB sensitivity : 2048 LSB/g, acc_mean values are on LSB sensitivity : 16 384 LSB/g so we divide by 8
	acc_z_offset = (ACC_1G - acc_z_mean) / ACC_SENSITIVITY;
	gyro_y_offset = - gyro_y_mean / GYRO_SENSITIVITY;			//gyro offset values are on LSB sensitivity : 32.8 LSB/(°/s), gyro_mean values are on LSB sensitivity : 131 LSB/(°/s) so we divide by 4

	while(1) {
		if (set_offset(acc_x_offset, acc_z_offset, gyro_y_offset)) {	//if we fail to set the offsets, we quit the function but we print that we failed
			io_error("TIMEOUT\n");
			return;
		}

		passCounter = 0;

		/* We re-calculate the mean values, to see if the precision of the offset is enough for us, if not, we loop again */

		calculate_mean(&acc_x_mean, &acc_z_mean, &gyro_y_mean);

		if (abs(acc_x_mean) <= ACC_SENSITIVITY) {
			passCounter+=1;
		} else {
			acc_x_offset -= acc_x_mean / ACC_SENSITIVITY;
		}
		if (abs(ACC_1G - acc_z_mean) <= (ACC_SENSITIVITY)) {
			passCounter+=2;
		} else {
			acc_z_offset -= (acc_z_mean - ACC_1G) / ACC_SENSITIVITY;
		}
		if (abs(gyro_y_mean) <= GYRO_SENSITIVITY) {
			passCounter+=GYRO_SENSITIVITY;
		} else {
			gyro_y_offset -= gyro_y_mean / GYRO_SENSITIVITY;
		}
		/*
		UART_PutINT(passCounter);
		UART_PutSTR("\n");
		UART_PutINT(acc_x_mean);
		UART_PutSTR("\n");
		UART_PutINT(acc_z_mean);
		UART_PutSTR("\n");
		UART_PutINT(gyro_y_mean);
		UART_PutSTR("\n\n");
		*/
		if (passCounter == 7) {	//if all the tests have succeeded, we got our offset values, we break the loop
			break;
		}

	}
	io_info("cali is finished");
	/*UART_PutSTR("Cali is finished\n");*/
}

uint8_t set_offset(int16_t acc_x_off, int16_t acc_z_off, int16_t gyro_y_off)
{
	uint8_t counter_timeout;


	int8_t acc_x_offsetH = acc_x_off >> 8;
	int8_t acc_x_offsetL = acc_x_off & 0x00FF;
	int8_t acc_z_offsetH = acc_z_off >> 8;
	int8_t acc_z_offsetL = acc_z_off & 0x00FF;
	int8_t gyro_y_offsetH = gyro_y_off >> 8;
	int8_t gyro_y_offsetL = gyro_y_off & 0x00FF;

	/* We write to the IMU to give him our current values for the offsets */

	counter_timeout = 0;

	while(I2C_Write_Blocking_1B(imu_address, ACC_X_OFF_H, acc_x_offsetH) == 2) {
		counter_timeout++;
		if (counter_timeout == 2){
			return 1;
		}
	}

	while(I2C_Write_Blocking_1B(imu_address, ACC_X_OFF_L, acc_x_offsetL) == 2) {
		counter_timeout++;
		if (counter_timeout == 2){
			return 1;
		}
	}

	while(I2C_Write_Blocking_1B(imu_address, ACC_Z_OFF_H, acc_z_offsetH) == 2) {
		counter_timeout++;
		if (counter_timeout == 2){
			return 1;
		}
	}

	while(I2C_Write_Blocking_1B(imu_address, ACC_Z_OFF_L, acc_z_offsetL) == 2) {
		counter_timeout++;
		if (counter_timeout == 2){
			return 1;
		}
	}

	while(I2C_Write_Blocking_1B(imu_address, GYRO_Y_OFF_H, gyro_y_offsetH) == 2) {
		counter_timeout++;
		if (counter_timeout == 2){
			return 1;
		}
	}

	while(I2C_Write_Blocking_1B(imu_address, GYRO_Y_OFF_L, gyro_y_offsetL) == 2) {
		counter_timeout++;
		if (counter_timeout == 2){
			return 1;
		}
	}
	return 0;
}

#endif //#ifndef CALIB_ME
