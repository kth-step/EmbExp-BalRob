/*
 * motor.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"
#include "dev/hw.h"


////////////////// configuration ////////////////////
//#define USE_DRV8833
#define USE_L298

#define TMR_PCLK		(12 * 1000 * 1000)
#define TMR_PRESCALE	(1)
#define TMR_FREQ		(2000)

////////////////// pinouts for motor circuits ////////////////////
/*
==========================================================================================================
*/

/*
Motor (DRV8833)
--------------
nFAULT
- R/PIO1_0/AD1/CT32B1_CAP0
IN1-2
- PIO0_8/MISO0/CT16B0_MAT0
- PIO0_9/MOSI0/CT16B0_MAT1
IN3-4
- R/PIO1_1/AD2/CT32B1_MAT0
- R/PIO1_2/AD3/CT32B1_MAT1
*/

/*
==========================================================================================================
*/

/*
Motor (L298) 2kHz max
------------
PWML = PIO0_8/CT16B0_MAT0
INL1 = PIO0_9
INL2 = PIO3_1

PWMR = PIO1_1/CT32B1_MAT0
INR1 = PIO1_2
INR2 = PIO 3_2

 */
/*
==========================================================================================================
*/

////////////////// common timer-related helper functions ////////////////////
#define MOTOR_MAX_VAL (TMR_PCLK/TMR_PRESCALE/TMR_FREQ)
#if MOTOR_MAX_VAL >= (32768-1)
#error "something might not fit in the timer registers, check this"
#endif
#define MOTOR_START_VAL (MOTOR_MAX_VAL * 18 / 60)

void motor_timer_init() {
	// initialize timers and setup for PWM
	// setup timer match
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);
	LPC_TMR16B0->PR = TMR_PRESCALE - 1; // no prescaler
	LPC_TMR16B0->MR3 = (MOTOR_MAX_VAL) - 1; // period
	LPC_TMR16B0->MR0 = LPC_TMR16B0->MR3 + 1;
	LPC_TMR16B0->MCR = (1 << 10);
	LPC_TMR16B0->PWMC = 1 << 0;

	LPC_TMR16B0->TCR = 1;



	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 10);
	LPC_TMR32B1->PR = TMR_PRESCALE - 1; // no prescaler
	LPC_TMR32B1->MR3 = (MOTOR_MAX_VAL) - 1; // period
	LPC_TMR32B1->MR0 = LPC_TMR32B1->MR3 + 1;
	LPC_TMR32B1->MCR = (1 << 10);
	LPC_TMR32B1->PWMC = 1 << 0;

	LPC_TMR32B1->TCR = 1;
}

int motor_prep_input(int r) {
	char sign = r < 0;
	if (sign)
		r = -r;

	r = r > MOTOR_MAX_VAL ? MOTOR_MAX_VAL : r;

	r = r < MOTOR_START_VAL ? (r < MOTOR_START_VAL / 2 ? 0 : MOTOR_START_VAL) : r;


#ifdef USE_DRV8833
	if (!sign)
#else
	if (1)
#endif
	{
		r = MOTOR_MAX_VAL - r;
	}

	return r;
}



////////////////// driver code ////////////////////
void motor_init()
{
#ifdef USE_DRV8833
	//PIO1_0 (nFAULT)
	LPC_IOCON->R_PIO1_0  &= ~0x07;
	LPC_IOCON->R_PIO1_0  |= 0x01;
	hw_gpio_set_dir(1,0,0);
#endif

	// left motor
	//PIO0_8 (PWML)
	LPC_IOCON->PIO0_8  &= ~0x07;
	LPC_IOCON->PIO0_8  |= 0x02;   // PIO0_8=0, CT16B0_MAT0=2
	//hw_gpio_set_dir(0,8,1);
	//hw_gpio_set(0,8,0);
	//PIO0_9
	LPC_IOCON->PIO0_9  &= ~0x07;
	LPC_IOCON->PIO0_9  |= 0x00;
	hw_gpio_set_dir(0,9,1);
	hw_gpio_set(0,9,0);
#ifdef USE_L298
	//PIO3_1
	LPC_IOCON->PIO3_1  &= ~0x07;
	LPC_IOCON->PIO3_1  |= 0x00;
	hw_gpio_set_dir(3,1,1);
	hw_gpio_set(3,1,0);
#endif

	// right motor
	//PIO1_1 (PWMR)
	LPC_IOCON->R_PIO1_1  &= ~0x07;
	LPC_IOCON->R_PIO1_1  |= 0x03;   // PIO1_1=1, CT32B1_MAT0=3
	//hw_gpio_set_dir(1,1,1);
	//hw_gpio_set(1,1,0);
	//PIO1_2
	LPC_IOCON->R_PIO1_2  &= ~0x07;
	LPC_IOCON->R_PIO1_2  |= 0x01;
	hw_gpio_set_dir(1,2,1);
	hw_gpio_set(1,2,0);
#ifdef USE_L298
	//PIO3_2
	LPC_IOCON->PIO3_2  &= ~0x07;
	LPC_IOCON->PIO3_2  |= 0x00;
	hw_gpio_set_dir(3,2,1);
	hw_gpio_set(3,2,0);
#endif

	motor_timer_init();
}

char motor_get_status() {
#ifdef USE_DRV8833
	// nFAULT
	return !hw_gpio_get(1,0);
#endif
#ifdef USE_L298
	return 0;
#endif
}

void motor_set_l(int l) {
	if (l < 0) {
		hw_gpio_set(0,9,1);
#ifdef USE_L298
		hw_gpio_set(3,1,0);
#endif
	} else {
		hw_gpio_set(0,9,0);
#ifdef USE_L298
		hw_gpio_set(3,1,1);
#endif
	}

	l = motor_prep_input(l);

	if (l == MOTOR_MAX_VAL) {
		LPC_TMR16B0->MR0 = LPC_TMR16B0->MR3 + 1;
	} else {
		LPC_TMR16B0->MR0 = l;
	}
}

void motor_set_r(int r) {
	if (r < 0) {
		hw_gpio_set(1,2,0);
#ifdef USE_L298
		hw_gpio_set(3,2,1);
#endif
	} else {
		hw_gpio_set(1,2,1);
#ifdef USE_L298
		hw_gpio_set(3,2,0);
#endif
	}

	r = motor_prep_input(r);

	if (r == MOTOR_MAX_VAL) {
		LPC_TMR32B1->MR0 = LPC_TMR32B1->MR3 + 1;
	} else {
		LPC_TMR32B1->MR0 = r;
	}
}

void motor_set(int l, int r) {
	motor_set_l(l);
	motor_set_r(r);
}

void motor_set_f(float l, float r) {
	motor_set(l*MOTOR_MAX_VAL, r*MOTOR_MAX_VAL);
}


