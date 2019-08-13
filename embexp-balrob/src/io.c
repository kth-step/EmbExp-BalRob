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

int in_state = 0;
uint8_t in_ch;
uint32_t in_data;
int in_handle() {
	int c = uart_read();
	if (c < 0) return -1;

	switch (in_state) {
	case 0:
		if (c == 0x55)
			in_state++;
		else
			return -2;
		break;
	case 1:
		in_ch = c;
		in_data = 0;
		in_state++;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		in_data |= c << ((in_state-2) * 8);
		in_state++;
		break;
	case 6:
		in_state = 0;
		if (c == 0xFF)
			return in_ch;
		else
			return -4;
		break;
	}

	return -3;
}

/*
--------------------------------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdio.h>

void printf_simple(char *fmt, ...);
void printf_simple_core(char *fmt, va_list args);

char buffer_txt[300];
void out_data(uint8_t ch, uint32_t data) {
	//snprintf(buffer_txt, 300, "hello %d, %lu", ch, data);
	while (uart_write(0x55));
	while (uart_write(ch));
	while (uart_write(data >> (0 * 8)));
	while (uart_write(data >> (1 * 8)));
	while (uart_write(data >> (2 * 8)));
	while (uart_write(data >> (3 * 8)));
	while (uart_write(0xFF));
}

void out_info(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

	while (uart_write(0x55));
	while (uart_write(0));
    printf_simple_core(fmt, args);
	printf_simple("\r\n");
	while (uart_write(0xFF));

    va_end(args);
}

void out_debug(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

	while (uart_write(0x55));
	while (uart_write(1));
	printf_simple("DEBUG: ");
    printf_simple_core(fmt, args);
	printf_simple("\r\n");
	while (uart_write(0xFF));

    va_end(args);
}

void out_error(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

	while (uart_write(0x55));
	while (uart_write(2));
	printf_simple("ERROR: ");
    printf_simple_core(fmt, args);
	printf_simple("\r\n");
	while (uart_write(0xFF));

    va_end(args);

    // loop forever because of the error
	while(1);
}




/*
--------------------------------------------------------------------------------------------
 */

#include <unistd.h>

// Function returns number of unwritten bytes if error, otherwise 0 for success
int _write(int iFileHandle, char *pcBuffer, int iLength) {
	if (iFileHandle != STDOUT_FILENO) {
	  return -1;
	}

	for (int i = 0; i < iLength; i++) {
		while (uart_write(pcBuffer[i]));
	}
	return 0;
}


// Function returns number of characters read, stored in pcBuffer
int _read(int iFileHandle, char *pcBuffer, int iLength) {
	if (iFileHandle != STDIN_FILENO) {
	  return -1;
	}

	char c;
	while ((c = uart_read()) < 0);
	switch (c) {
	case '\n':
	case '\r':
		while (uart_write('\r'));
		while (uart_write('\n'));
		break;
	default:
		while (uart_write(c));
	}

	pcBuffer[0] = c;

	return 1;
}



/*
--------------------------------------------------------------------------------------------
 */




void printf_new(const char *fmt, ...);

int uart_write(char c);
void uart_putchar(char c) {
	while (uart_write(c));
}

static void printf_string(char *str)
{
    while(*str) {
    	while (uart_write(*str));
    	str++;
    }
}

/*
static void printf_string(char *str)
{
    if(!str) str = "(null)";
    while(*str) uart_putchar(*str++);
}
*/

static void printf_int(int i)
{
    int f = 0, neg = 0;
    char buffer[28];

    if(i < 0) {
        neg ++;
        i = - i;
    }
    do {
        buffer[f++] = '0' + (i % 10);
        i /= 10;
    } while(i);

    if(neg) buffer[f++] = '-';

    while(f) {
        uart_putchar( buffer[--f]);
    }
}

static void printf_hex(uint32_t n, int size)
{
    uint64_t h;
    int i;

    for(i = size * 8; i; ) {
        i -= 4;
        h = (n >> i) & 15;

        if(h < 10) h += '0';
        else h += 'A' - 10;

        uart_putchar(h);
    }
}

static void printf_bin(uint32_t n)
{
    int i;
    for(i = 32; i != 0; i--) {
        if( (i != 32) && !(i & 3)) uart_putchar('_');
        uart_putchar( (n >> 31) ? '1' : '0');
        n <<= 1;
    }
}

static void printf_float(float f)
{
	int32_t a = (int32_t)(f * 1000);
	printf_int(a / 1000);
	uart_putchar('.');
	printf_int((a < 0 ? a * (-1) : a) % 1000);
}

// --------------------------------------------------------------
void printf_simple(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf_simple_core(fmt, args);

    va_end(args);
}



void printf_simple_core(char *fmt, va_list args) {
    int c;
    for(;;) {
        c = *fmt;
        if(c == '\0') break;

        fmt ++;
        if(c == '%') {
            c = *fmt;
            fmt++;

            // sanity check?
            if(c == '\0') {
                uart_putchar(c);
                break;
            }

            switch(c) {
            case 'c':
                uart_putchar(va_arg(args, int));
                break;
            case 's':
                printf_string(va_arg(args, char *));
                break;
            case 'i':
            case 'd':
                printf_int(va_arg(args, int));
                break;
            case 'x':
                printf_hex(va_arg(args, uint32_t), 4);
                break;
            case 'b':
                printf_bin(va_arg(args, uint32_t));
                break;
            case 'f':
                printf_float(va_arg(args, double));
                break;
            case '%':
                uart_putchar(c);
                break;
            default:
                uart_putchar('%');
                uart_putchar(c);
            }

        } else uart_putchar(c);
    }
}

