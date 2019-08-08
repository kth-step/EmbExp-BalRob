/*
 * io.h
 *
 *  Created on: 8 aug. 2019
 *      Author: andreas
 */

#ifndef IO_H_
#define IO_H_

void io_init();

void io_info(char* str);
void io_debug(char* str);
void io_error(char* str);



#include <stdarg.h>

void printf_new(const char *fmt, ...);


#endif /* IO_H_ */
