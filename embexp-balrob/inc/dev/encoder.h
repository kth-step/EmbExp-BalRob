#ifndef DEV_ENCODER_H_
#define DEV_ENCODER_H_

#include <stdint.h>

extern volatile int32_t encoder_values[2];

void encoder_init();

#endif /* DEV_ENCODER_H_ */

