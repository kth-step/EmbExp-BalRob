/*
 * imu.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"
#include <dev/i2c.h>
#include <dev/hw.h>

#include <io.h>


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

#define IMU_WRITE_AND_RETURN(a,v) {uint8_t temp;if ((temp = imu_write(a, v))) return temp;}


uint8_t set_offset_(int16_t acc_x_off, int16_t acc_z_off, int16_t gyro_y_off)
{
	int8_t acc_x_offsetH = acc_x_off >> 8;
	int8_t acc_x_offsetL = acc_x_off & 0x00FF;
	int8_t acc_z_offsetH = acc_z_off >> 8;
	int8_t acc_z_offsetL = acc_z_off & 0x00FF;
	int8_t gyro_y_offsetH = gyro_y_off >> 8;
	int8_t gyro_y_offsetL = gyro_y_off & 0x00FF;

#define ACC_X_OFF_H 			0x06
#define ACC_X_OFF_L				0x07
#define ACC_Z_OFF_H				0x0A
#define ACC_Z_OFF_L				0x0B
#define GYRO_Y_OFF_H			0x15
#define GYRO_Y_OFF_L			0x16

	IMU_WRITE_AND_RETURN(ACC_X_OFF_H, acc_x_offsetH);
	IMU_WRITE_AND_RETURN(ACC_X_OFF_L, acc_x_offsetL);

	IMU_WRITE_AND_RETURN(ACC_Z_OFF_H, acc_z_offsetH);
	IMU_WRITE_AND_RETURN(ACC_Z_OFF_L, acc_z_offsetL);

	IMU_WRITE_AND_RETURN(GYRO_Y_OFF_H, gyro_y_offsetH);
	IMU_WRITE_AND_RETURN(GYRO_Y_OFF_L, gyro_y_offsetL);

	return 0;
}

uint8_t imu_init(uint8_t wint)
{
	//Init AD0, pull down valuesistor
	LPC_IOCON->PIO1_10  &= ~0x1F;
	LPC_IOCON->PIO1_10  |= 0x08;

	//Init i2c bus
	I2CInit(I2CMASTER);

	// sample rate 333Hz ( we enable DLPF afterwards, so 1kHz clock base )
	IMU_WRITE_AND_RETURN(0x19, (5) - 1);
	// DLPF enabled, 100Hz
	IMU_WRITE_AND_RETURN(0x1A, 0x3);

	// Gyro scale range set to +- 250°/s, LSB sensitivity 131 LSB/(°/s)
	IMU_WRITE_AND_RETURN(0x1B, 0x00);
	// Acc scale range set to +- 2g, LSB sensitivity 16 384 LSB/g
	IMU_WRITE_AND_RETURN(0x1C, 0x00);

	// interrupt pin high active, stay active until data read
	IMU_WRITE_AND_RETURN(0x37, 0x30);

	// interrupt when new data is ready
	IMU_WRITE_AND_RETURN(0x38, 0x01);

	// Power management (here clock source is Internal 8MHz oscillator)
	IMU_WRITE_AND_RETURN(0x6B, 0x00);

	// configure offsets with precalibrated values
//#define BOT_LEGO
#define BOT_MINI

#ifdef BOT_LEGO
#define ACC_X_OFF				-1251
#define ACC_Z_OFF				910
#define GYRO_Y_OFF				-18
#endif
#ifdef BOT_MINI
#define ACC_X_OFF				-2214
#define ACC_Z_OFF				1096
#define GYRO_Y_OFF				18
#endif
	//calculate_offset(ACC_X_OFF, ACC_Z_OFF, GYRO_Y_OFF);
	uint8_t temp;
	if ((temp = set_offset_(ACC_X_OFF, ACC_Z_OFF, GYRO_Y_OFF)))
		return temp;

	// setup for interrupts
	// ---------------------------------
	// init gpio for imu int pin, with pull-up
	LPC_IOCON->PIO1_11  &= ~0x07;
	LPC_IOCON->PIO1_11  |= 0x00;
	hw_gpio_set_dir(1,11,0);

	// interrupt as edge sensitive and rising edge interrupt
	LPC_GPIO1->IS = 0;
	LPC_GPIO1->IBE = 0;
	LPC_GPIO1->IEV = (1 << 11);
	/*
	// interrupt as level sensitive, high active
	LPC_GPIO1->IS = (1 << 11);
	LPC_GPIO1->IBE = 0;
	LPC_GPIO1->IEV = (1 << 11);
	*/
	// Enable GPIO pin interrupt
	LPC_GPIO1->IE = (1 << 11);


	// read once to clear data ready interrupt
	imu_read_values();

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	NVIC_SetPriority(EINT1_IRQn,3);
	if (wint)
		NVIC_EnableIRQ(EINT1_IRQn);
	else
		NVIC_DisableIRQ(EINT1_IRQn);

	return 0;
}

void imu_wait_new_data() {
	while (!hw_gpio_get(1,11));
}

__attribute__ ((weak)) void imu_handler(uint8_t noyield) {
	imu_read_values();
}

uint8_t last_noyield = 0;
void PIOINT1_IRQHandler(void)
{
	// clear interrupt for pin
	LPC_GPIO1->IC = (1 << 11); // has no effect if in level sensitive mode

	// run imu_handler, if level sensitive imu needs to be read in the handler
	imu_handler(last_noyield);

	// check whether handler was too slow (is the interrupt already set again?)
	last_noyield = (LPC_GPIO1->MIS & (1 << 11)) >> 11;
}



