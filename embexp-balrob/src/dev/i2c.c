#include <dev/i2c.h>
#include <LPC11xx.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

volatile uint32_t I2CMasterState = I2C_IDLE;
volatile uint32_t I2CSlaveState = I2C_IDLE;

volatile uint32_t I2CMode;

volatile uint8_t I2CMasterBuffer[BUFSIZE];
volatile uint8_t I2CSlaveBuffer[BUFSIZE];
volatile uint32_t I2CCount = 0;
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;

volatile uint8_t ask_for_new_value = 0;
volatile uint8_t read_available = 1;
volatile uint8_t I2CWrite = 0;
volatile uint32_t timeout = 0;

extern volatile int16_t imu_values[7];
volatile uint8_t ask_for_reading_while_busy = 0;
extern const uint8_t imu_address;


uint8_t I2C_Write_Blocking(uint8_t dev_id, uint8_t reg, uint8_t* ba, int n)
{
	if ((n >= BUFSIZE-2) | (n < 0))
		return 1;

	I2CMasterBuffer[0] = dev_id << 1;
	I2CMasterBuffer[1] = reg;
	for (int i = 0; i < n; i++) {
		I2CMasterBuffer[2 + i] = ba[i];
	}
	I2CWriteLength = 2 + n;
	I2CReadLength = 0;

	I2CMasterState = I2C_BUSY;
	I2CWrite = 1;

	timeout = 0;
	LPC_I2C->CONSET = I2CONSET_STA;
	while ((I2CMasterState == I2C_BUSY) && (timeout < MAX_TIMEOUT)) {
		timeout++;
	}

	if (I2CMasterState == I2C_BUSY) {
		return 2;
	} else {
		return 0;
	}
}

uint8_t I2C_Write_Blocking_1B(uint8_t dev_id, uint8_t reg, uint8_t b) {
	return I2C_Write_Blocking(dev_id, reg, &b, 1);
}

void I2C_Read_nBlocking(uint8_t dev_id, uint8_t reg, int n)
{
	if ((n >= BUFSIZE) | (n < 0))
		return;

	I2CMasterBuffer[0] = dev_id << 1;
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = dev_id << 1 | 0x01;
	I2CWriteLength = 2;
	I2CReadLength = n;

	I2CWrite = 0;
	I2CMasterState = I2C_BUSY;

	LPC_I2C->CONSET = I2CONSET_STA;

	read_available = 0;
	while (I2CMasterState == I2C_BUSY);

	return;

}

void I2C_Read_Blocking(uint8_t dev_id, uint8_t reg, int n)
{
	if ((n >= BUFSIZE) | (n < 0))
		return;

	I2CMasterBuffer[0] = dev_id << 1;
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = dev_id << 1 | 0x01;
	I2CWriteLength = 2;
	I2CReadLength = n;

	I2CWrite = 0;
	I2CMasterState = I2C_BUSY;

	LPC_I2C->CONSET = I2CONSET_STA;

	read_available = 0;
	while (I2CMasterState == I2C_BUSY);

}

void I2C_IRQHandler(void)
{

	if (I2CWrite == 1) {
		timeout = 0;
		I2CDriver();
		if (I2CMasterState != I2C_BUSY) {
			I2CWrite = 0;
		}
	} else { // I2CWrite == 0
		I2CDriver();
		if (I2CMasterState != I2C_BUSY) {
			imu_values[0] = ((I2CSlaveBuffer[0]  >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[0]  << 8) | (I2CSlaveBuffer[1]);
			imu_values[1] = ((I2CSlaveBuffer[2]  >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[2]  << 8) | (I2CSlaveBuffer[3]);
			imu_values[2] = ((I2CSlaveBuffer[4]  >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[4]  << 8) | (I2CSlaveBuffer[5]);
			imu_values[3] =  ((I2CSlaveBuffer[6]  >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[6]  << 8) | (I2CSlaveBuffer[7]);
			imu_values[4] = ((I2CSlaveBuffer[8]  >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[8]  << 8) | (I2CSlaveBuffer[9]);
			imu_values[5] = ((I2CSlaveBuffer[10] >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[10] << 8) | (I2CSlaveBuffer[11]);
			imu_values[6] = ((I2CSlaveBuffer[12] >> 7) ? 0xFFFF0000 : 0x0) | (I2CSlaveBuffer[12] << 8) | (I2CSlaveBuffer[13]);

			if (ask_for_new_value == 1) {
				ask_for_new_value = 0;
				I2C_Read_nBlocking(imu_address, 0x3B, 14);
			} else { //ask_for_new_value == 0
				read_available = 1;
			}
		}
	}
}

void I2CDriver()
{
	uint8_t StatValue;
	StatValue = LPC_I2C->STAT;
	switch ( StatValue )
	{
	case 0x08:			/* A Start condition is issued. */
		WrIndex = 0;
		LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	break;

	case 0x10:			/* A repeated started is issued */
		RdIndex = 0;
		/* Send SLA with R bit set, */
		LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	break;

	case 0x18:			/* Regardless, it's a ACK */
		if ( I2CWriteLength == 1 )
		{
		  LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
		  I2CMasterState = I2C_NO_DATA;
		}
		else
		{
		  LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
		if ( WrIndex < I2CWriteLength )
		{
		  LPC_I2C->DAT = I2CMasterBuffer[WrIndex++]; /* this should be the last one */
		}
		else
		{
		  if ( I2CReadLength != 0 )
		  {
			LPC_I2C->CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
		  }
		  else
		  {
			LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
			I2CMasterState = I2C_OK;
		  }
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x30:
		LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
		I2CMasterState = I2C_NACK_ON_DATA;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x40:	/* Master Receive, SLA_R has been sent */
		if ( (RdIndex + 1) < I2CReadLength )
		{
		  /* Will go to State 0x50 */
		  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
		}
		else
		{
		  /* Will go to State 0x58 */
		  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK after data is received */
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
		I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
		if ( (RdIndex + 1) < I2CReadLength )
		{
		  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
		}
		else
		{
		  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x58:
		I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
		I2CMasterState = I2C_OK;
		LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
		LPC_I2C->CONCLR = I2CONCLR_SIC;	/* Clear SI flag */
	break;

	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
		LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
		I2CMasterState = I2C_NACK_ON_ADDRESS;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x38:		/* Arbitration lost, in this example, we don't
					deal with multiple master situation */
	default:
		I2CMasterState = I2C_ARBITRATION_LOST;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;
	}
}

uint32_t I2CInit( uint32_t I2cMode )
{
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);

  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;	/*  I2C I/O config */
  LPC_IOCON->PIO0_4 |= 0x01;		/* I2C SCL */
  LPC_IOCON->PIO0_5 &= ~0x3F;
  LPC_IOCON->PIO0_5 |= 0x01;		/* I2C SDA */
  /* IOCON may change in the next release, save change for future references. */
//  LPC_IOCON->PIO0_4 |= (0x1<<10);	/* open drain pins */
//  LPC_IOCON->PIO0_5 |= (0x1<<10);	/* open drain pins */

  /*--- Clear flags ---*/
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;

  /*--- Reset registers ---*/
  /*
  LPC_I2C->SCLL   = I2SCLL_SCLL;
  LPC_I2C->SCLH   = I2SCLH_SCLH;
  */
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;

  if ( I2cMode == I2CSLAVE )
  {
	LPC_I2C->ADR0 = PCF8594_ADDR;
  }

  /* Enable the I2C Interrupt */
  NVIC_SetPriority(I2C_IRQn,2);
  NVIC_EnableIRQ(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN;
  return( TRUE );
}
