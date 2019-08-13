/*
 * io.c
 *
 *  Created on: 8 aug. 2019
 *      Author: andreas
 */

#include <dev/uart.h>

#include <stdint.h>

void io_init() {
	uart_init();
}

/*
--------------------------------------------------------------------------------------------
 */

int in_idx = 0;
uint8_t in_buffer[2+1+1+255+2];
uint8_t in_ch;
uint32_t in_data_len;
int32_t in_data;
int in_handle() {
	int c = uart_read();
	if (c < 0) return -1;
	uint8_t b = c;
	in_buffer[in_idx] = b;

	switch (in_idx) {
	case 0:
		if (b == 0x55)
			in_idx++;
		else
			return -2;
		return -3;
	case 1:
		if (b == 0xAA)
			in_idx++;
		else {
			in_idx = 0;
			return -2;
		}
		return -3;
	case 2:
		in_ch = b;
		in_data = 0;
		in_idx++;
		return -3;
	case 3:
		in_data_len = b;
		in_idx++;
		return -3;
	default:
		break;
	}

	uint32_t in_data_idx = in_idx - 4;
	if (in_data_idx < in_data_len) {
		if (in_data_idx < 4)
			in_data |= b << ((in_data_idx) * 8);

		in_idx++;
		return -3;
	}

	if (in_data_idx == in_data_len + 0) {
		if (b == 0x88) {
			in_idx++;
			return -3;
		} else {
			in_idx = 0;
			return -4;
		}
	}

	if (in_data_idx == in_data_len + 1) {
		if (b == 0x11) {
			in_idx = 0;
			return in_ch;
		} else {
			in_idx = 0;
			return -4;
		}
	}

	// we should never get here!
	in_idx = 0;
	return -255;
}

/*
--------------------------------------------------------------------------------------------
 */

void out_data(uint8_t ch, uint8_t* data, uint8_t len) {
	while (uart_write(0x55));
	while (uart_write(0xAA));

	while (uart_write(ch));
	while (uart_write(len));

	for (int i = 0; i < len; i++)
		while (uart_write(data[i]));

	while (uart_write(0x88));
	while (uart_write(0x11));
}

void out_data_int(uint8_t ch, int32_t data) {
	out_data(ch, (uint8_t*)&data, 4);
}

#include <stdarg.h>
#include <stdio.h>

#define OUT_MAX_CHAR (255)
char out_buffer[OUT_MAX_CHAR+1];
void out_vprintf(uint8_t ch, char *fmt, va_list args) {
	int n = vsnprintf(out_buffer, OUT_MAX_CHAR+1, fmt, args);

	if (n < 0)
		while (1);

	if (n > OUT_MAX_CHAR) {
		out_buffer[OUT_MAX_CHAR - 3] = '.';
		out_buffer[OUT_MAX_CHAR - 2] = '.';
		out_buffer[OUT_MAX_CHAR - 1] = '.';
		out_buffer[OUT_MAX_CHAR - 0] = 0;
		n = OUT_MAX_CHAR;
	}

	out_data(ch, (uint8_t*)out_buffer, (uint8_t)n);
}

void out_info(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    out_vprintf(0, fmt, args);

    va_end(args);
}

void out_debug(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    out_vprintf(1, fmt, args);

    va_end(args);
}

void out_error(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    out_vprintf(2, fmt, args);

    va_end(args);

    // loop forever because of the error
	while(1);
}



