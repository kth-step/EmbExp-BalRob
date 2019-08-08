/*
 * imu.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"
#include <dev/i2c.h>
#include "dev/hw.h"

#include <stdio.h>

/*
IMU (MPU6050)
--------------
SCL&SDA
- PIO0_4/SCL
- PIO0_5/SDA
AD0&INT
- PIO1_10/AD6/CT16B1_MAT1
- PIO1_11/AD7
 */



/*
--------------------------------------------------------------------------------------------
 */


const uint8_t imu_address = 0x68;
volatile int16_t imu_values[7];


void imu_read_values()
{
	//Ask for the 14 bytes stocked after register 0x3B
	I2C_Read_nBlocking(imu_address, 0x3B, 14);
}

uint8_t imu_write(uint8_t addr, uint8_t val) {
	uint8_t counter_timeout = 0;

	while(I2C_Write_Blocking_1B(imu_address, addr, val) == 2) {
		//UART_PutSTR("Timeout on writing into the IMU\r\n");
		counter_timeout++;
		if (counter_timeout == 2){
			return addr;
		}
	}

	return 0;
}

uint8_t imu_init()
{
	uint8_t temp;
	//Init AD0, pull down valuesistor
	LPC_IOCON->PIO1_10  &= ~0x1F;
	LPC_IOCON->PIO1_10  |= 0x08;

	// init gpio for imu int pin
	LPC_IOCON->PIO1_11  &= ~0x07;
	LPC_IOCON->PIO1_11  |= 0x00;
	hw_gpio_set_dir(1,11,0);

	//Init i2c bus
	I2CInit(I2CMASTER);

	// sample rate 333Hz ( we enable DLPF afterwards, so 1kHz clock base )
	if ((temp = imu_write(0x19, (3) - 1)))
		return temp;
	// DLPF enabled, 100Hz
	if ((temp = imu_write(0x1A, 0x2)))
		return temp;

	// Gyro scale range set to +- 250°/s, LSB sensitivity 131 LSB/(°/s)
	if ((temp = imu_write(0x1B, 0x00)))
		return temp;
	// Acc scale range set to +- 2g, LSB sensitivity 16 384 LSB/g
	if ((temp = imu_write(0x1C, 0x00)))
		return temp;

	// interrupt pin high active, stay active until data read
	if ((temp = imu_write(0x37, 0x30)))
		return temp;

	// interrupt when new data is ready
	if ((temp = imu_write(0x38, 0x01)))
		return temp;

	// Power management (here clock source is Internal 8MHz oscillator)
	if ((temp = imu_write(0x6B, 0x00)))
		return temp;

	// read once to clear data ready interrupt
	imu_read_values();

	printf("imu init done.\r\n");
	return 0;
}

void imu_wait_new_data() {
	while (!hw_gpio_get(1,11));
}




