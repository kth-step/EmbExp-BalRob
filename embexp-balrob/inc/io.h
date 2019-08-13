/*
 * io.h
 *
 *  Created on: 8 aug. 2019
 *      Author: andreas
 */

#ifndef IO_H_
#define IO_H_

void io_init();


// returns negative number if there is no data available, otherwise channel id
int in_handle();
// data that has been produced last
extern uint32_t in_data;


void out_data(uint8_t ch, uint32_t data);

void out_info(char *fmt, ...);
void out_debug(char *fmt, ...);
void out_error(char *fmt, ...);


#endif /* IO_H_ */
