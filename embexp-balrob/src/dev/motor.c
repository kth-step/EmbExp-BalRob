/*
 * motor.c
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#include "LPC11xx.h"
#include "dev/hw.h"

#include <robot_params.h>

////////////////// configuration ////////////////////
#ifdef BOT_LEGO
#define USE_L298
#endif
#ifdef BOT_MINI
#define USE_DRV8833
#endif
#ifdef BOT_BALPEN
#define USE_L298
#endif

#define TMR_PCLK		(12 * 1000 * 1000)
#define TMR_PRESCALE	(1)
#define TMR_FREQ		(30000)

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
INL2 = PIO0_2

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
//#define MOTOR_START_VAL (MOTOR_MAX_VAL * 5 / 60)

void KEEPINFLASH motor_timer_init() {
	// initialize timers and setup for PWM
	// setup timer match
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);
	LPC_TMR16B0->PR = TMR_PRESCALE - 1; // no prescaler
	LPC_TMR16B0->MR3 = (MOTOR_MAX_VAL) - 1; // period
	LPC_TMR16B0->MR0 = LPC_TMR16B0->MR3 + 1;
	LPC_TMR16B0->MR1 = LPC_TMR16B0->MR3 + 1;
	LPC_TMR16B0->MCR = (1 << 10);
	LPC_TMR16B0->PWMC = (1 << 0) | (1 << 1);

	LPC_TMR16B0->TCR = 1;



	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 10);
	LPC_TMR32B1->PR = TMR_PRESCALE - 1; // no prescaler
	LPC_TMR32B1->MR3 = (MOTOR_MAX_VAL) - 1; // period
	LPC_TMR32B1->MR0 = LPC_TMR32B1->MR3 + 1;
	LPC_TMR32B1->MR1 = LPC_TMR32B1->MR3 + 1;
	LPC_TMR32B1->MCR = (1 << 10);
	LPC_TMR32B1->PWMC = (1 << 0) | (1 << 1);

	LPC_TMR32B1->TCR = 1;
}

int motor_prep_input(int r);
int motor_prep_input(int r) {
	char sign = r < 0;
	if (sign)
		r = -r;

	r = r > MOTOR_MAX_VAL ? MOTOR_MAX_VAL : r;

#ifdef MOTOR_START_VAL
	r = r < MOTOR_START_VAL ? (r < MOTOR_START_VAL / 2 ? 0 : MOTOR_START_VAL) : r;
#endif

	r = MOTOR_MAX_VAL - r;

	return r;
}



////////////////// driver code ////////////////////
void KEEPINFLASH motor_init()
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
#ifdef USE_DRV8833
	//PIO0_9
	LPC_IOCON->PIO0_9  &= ~0x07;
	LPC_IOCON->PIO0_9  |= 0x02;
#endif
#ifdef USE_L298
	//PIO0_9
	LPC_IOCON->PIO0_9  &= ~0x07;
	LPC_IOCON->PIO0_9  |= 0x00;  // PIO0_9=0, CT16B0_MAT1=2
	hw_gpio_set_dir(0,9,1);
	hw_gpio_set(0,9,0);
	//PIO0_2
	LPC_IOCON->PIO0_2  &= ~0x07;
	LPC_IOCON->PIO0_2  |= 0x00;
	hw_gpio_set_dir(0,2,1);
	hw_gpio_set(0,2,0);
#endif

	// right motor
	//PIO1_1 (PWMR)
	LPC_IOCON->R_PIO1_1  &= ~0x07;
	LPC_IOCON->R_PIO1_1  |= 0x03;   // PIO1_1=1, CT32B1_MAT0=3
	//hw_gpio_set_dir(1,1,1);
	//hw_gpio_set(1,1,0);
#ifdef USE_DRV8833
	//PIO1_2
	LPC_IOCON->R_PIO1_2  &= ~0x07;
	LPC_IOCON->R_PIO1_2  |= 0x03;
#endif
#ifdef USE_L298
	//PIO1_2
	LPC_IOCON->R_PIO1_2  &= ~0x07;
	LPC_IOCON->R_PIO1_2  |= 0x01;   // PIO1_2=1, CT32B1_MAT1=3
	hw_gpio_set_dir(1,2,1);
	hw_gpio_set(1,2,0);
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

#ifdef USE_DRV8833
void motor_set_l(int l) {
	char sign = l < 0;
	l = motor_prep_input(l);

	if (l == MOTOR_MAX_VAL) {
		LPC_TMR16B0->MR0 = LPC_TMR16B0->MR3 + 1;
		LPC_TMR16B0->MR1 = LPC_TMR16B0->MR3 + 1;
	} else if (sign) {
		LPC_TMR16B0->MR0 = LPC_TMR16B0->MR3 + 1;
		LPC_TMR16B0->MR1 = l;
	} else {
		LPC_TMR16B0->MR0 = l;
		LPC_TMR16B0->MR1 = LPC_TMR16B0->MR3 + 1;
	}
}
void motor_set_r(int r) {
	char sign = r < 0;
	r = motor_prep_input(r);

	if (r == MOTOR_MAX_VAL) {
		LPC_TMR32B1->MR0 = LPC_TMR32B1->MR3 + 1;
		LPC_TMR32B1->MR1 = LPC_TMR32B1->MR3 + 1;
	} else if (sign) {
		LPC_TMR32B1->MR0 = r;
		LPC_TMR32B1->MR1 = LPC_TMR32B1->MR3 + 1;
	} else {
		LPC_TMR32B1->MR0 = LPC_TMR32B1->MR3 + 1;
		LPC_TMR32B1->MR1 = r;
	}
}
#endif
#ifdef USE_L298
void motor_set_l(int l) {
	if (l < 0) {
		hw_gpio_set(0,9,1);
		hw_gpio_set(0,2,0);
	} else {
		hw_gpio_set(0,9,0);
		hw_gpio_set(0,2,1);
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
		hw_gpio_set(3,2,1);
	} else {
		hw_gpio_set(1,2,1);
		hw_gpio_set(3,2,0);
	}

	r = motor_prep_input(r);

	if (r == MOTOR_MAX_VAL) {
		LPC_TMR32B1->MR0 = LPC_TMR32B1->MR3 + 1;
	} else {
		LPC_TMR32B1->MR0 = r;
	}
}
#endif // motor_set_l and motor_set_r for #ifdef USE_L298

void motor_set(int l, int r) {
	motor_set_l(l);
	motor_set_r(r);
}

void motor_set_f(float l, float r) {
/*
	if (l < 0.1f) {
		l = 0.1f;
	} else if (l < 0.2f) {
		l = 0.1f + (l - 0.1f) * 2;
	} else if (l < 0.4f) {
		l = 0.3f + (l - 0.3f) * 1.5f;
	}
*/

	motor_set(l*MOTOR_MAX_VAL, r*MOTOR_MAX_VAL);
}


