#include "LPC11xx.h"
#include "dev/hw.h"

#include <robot_params.h>

#include <dev/encoder.h>

/*
encoder0
  PIO2_8
  PIO2_10

encoder1
  PIO3_0
  PIO0_6

*/

#ifdef BOT_LEGO
#define ENABLE_ENCODER
#endif

volatile int32_t encoder_values[2] = {0};

void encoder_init() {
	encoder_values[0] = 0;
	encoder_values[1] = 0;

#ifdef ENABLE_ENCODER
  // encoder 0
	//PIO2_8
	LPC_IOCON->PIO2_8  = 0x420;
	hw_gpio_set_dir(2,8,0);
	//PIO2_10
	LPC_IOCON->PIO2_10  = 0x420;
	hw_gpio_set_dir(2,10,0);

  // encoder 1
	//PIO3_0
	LPC_IOCON->PIO3_0  = 0x420;
	hw_gpio_set_dir(3,0,0);
	//PIO0_6
	LPC_IOCON->PIO0_6  = 0x420;
	hw_gpio_set_dir(0,6,0);


  // encoder 0
	// interrupt as edge sensitive, rising and falling edge interrupt
	LPC_GPIO2->IS = 0;
	LPC_GPIO2->IBE = (1 << 8) | (1 << 10);
	// Enable GPIO pin interrupt
	LPC_GPIO2->IE = (1 << 8) | (1 << 10);


	NVIC_ClearPendingIRQ(EINT2_IRQn);
	NVIC_SetPriority(EINT2_IRQn,2);
	NVIC_EnableIRQ(EINT2_IRQn);


  // encoder 1_PIO0_6
	// interrupt as edge sensitive, rising and falling edge interrupt
	LPC_GPIO0->IS = 0;
	LPC_GPIO0->IBE = (1 << 6);
	// Enable GPIO pin interrupt
	LPC_GPIO0->IE = (1 << 6);


	NVIC_ClearPendingIRQ(EINT0_IRQn);
	NVIC_SetPriority(EINT0_IRQn,2);
	NVIC_EnableIRQ(EINT0_IRQn);
  // encoder 1_PIO3_0
	// interrupt as edge sensitive, rising and falling edge interrupt
	LPC_GPIO3->IS = 0;
	LPC_GPIO3->IBE = (1 << 0);
	// Enable GPIO pin interrupt
	LPC_GPIO3->IE = (1 << 0);


	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT3_IRQn,2);
	NVIC_EnableIRQ(EINT3_IRQn);
#endif
}

static uint8_t lastval_enc0 = 0x00;
void encoder0_compute() {
	uint8_t curval0 = hw_gpio_get(2,8);
	uint8_t curval1 = hw_gpio_get(2,10);

	//uint8_t lastval0 = (lastval_enc0 >> 0) & 1;
	uint8_t lastval1 = (lastval_enc0 >> 1) & 1;

	if (curval1 == lastval1) {
		if (curval1) {
			if (curval0)
				encoder_values[0]--;
			else
				encoder_values[0]++;
		} else {
			if (curval0)
				encoder_values[0]++;
			else
				encoder_values[0]--;
		}
	} else {
		if (curval0) {
			if (curval1)
				encoder_values[0]++;
			else
				encoder_values[0]--;
		} else {
			if (curval1)
				encoder_values[0]--;
			else
				encoder_values[0]++;
		}
	}

	lastval_enc0 = (curval1 << 1) | (curval0 << 0);
}

void PIOINT2_IRQHandler(void)
{
	// clear interrupt for pin
	LPC_GPIO2->IC = (1 << 8);
	LPC_GPIO2->IC = (1 << 10);

	encoder0_compute();

	//last_noyield = (LPC_GPIO1->MIS & (1 << 11)) >> 11;
}

static uint8_t lastval_enc1 = 0x00;
void encoder1_compute() {
	uint8_t curval0 = hw_gpio_get(3,0);
	uint8_t curval1 = hw_gpio_get(0,6);

	//uint8_t lastval0 = (lastval_enc1 >> 0) & 1;
	uint8_t lastval1 = (lastval_enc1 >> 1) & 1;

	if (curval1 == lastval1) {
		if (curval1) {
			if (curval0)
				encoder_values[1]--;
			else
				encoder_values[1]++;
		} else {
			if (curval0)
				encoder_values[1]++;
			else
				encoder_values[1]--;
		}
	} else {
		if (curval0) {
			if (curval1)
				encoder_values[1]++;
			else
				encoder_values[1]--;
		} else {
			if (curval1)
				encoder_values[1]--;
			else
				encoder_values[1]++;
		}
	}

	lastval_enc1 = (curval1 << 1) | (curval0 << 0);
}

void PIOINT0_IRQHandler(void)
{
	// clear interrupt for pin
	LPC_GPIO0->IC = (1 << 6);

	encoder1_compute();

	//last_noyield = (LPC_GPIO1->MIS & (1 << 11)) >> 11;
}

void PIOINT3_IRQHandler(void)
{
	// clear interrupt for pin
	LPC_GPIO3->IC = (1 << 0);

	encoder1_compute();

	//last_noyield = (LPC_GPIO1->MIS & (1 << 11)) >> 11;
}
