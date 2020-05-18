/*
 * robot_params.h
 *
 *  Created on: 14 aug. 2019
 *      Author: andreas
 */

#ifndef ROBOT_PARAMS_H_
#define ROBOT_PARAMS_H_

//#define BOT_LEGO
#define BOT_MINI

#define GCC_COMPAT_COMPILATION


#ifdef GCC_COMPAT_COMPILATION
#define DONTKNOWMATH
#define PUREPRINT
#endif


// IMU calibration data (offsets)
// ---------------------------------------------------
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



// sensor parameters
// ---------------------------------------------------
#ifdef BOT_LEGO
#define GYR_SCALE 390
#define ANGLETARGET -15
#endif
#ifdef BOT_MINI
#define GYR_SCALE 250
//#define ANGLETARGET 6.2
//#define ANGLETARGET 5.88
#define ANGLETARGET 8.44
#endif



// pid parameters
// ---------------------------------------------------
#ifdef BOT_LEGO
#define INIT_KP 0.2
#define INIT_KI 0.2
#define INIT_KD 0.0001
#endif
#ifdef BOT_MINI
/*
// somewhat working
#define INIT_KP 0.1
#define INIT_KI 0.2
#define INIT_KD 0.0001
*/
/*
// way better
#define INIT_KP 0.15
#define INIT_KI 0.9
#define INIT_KD 0.002
*/
//quite stable
/*
#define INIT_KP 0.15
#define INIT_KI 0.9
#define INIT_KD 0.003
*/
//seems even better
#define INIT_KP 0.15
#define INIT_KI 0.9
#define INIT_KD 0.00375
#endif

#define KEEPINFLASH __attribute__((__section__(".keepinflash")))
#define RELOADTEXTENTRY __attribute__((__section__(".reloadtextentry")))

#endif /* ROBOT_PARAMS_H_ */
