
#ifdef __BENCHMARK_MODE

#include <stdint.h>

#include "LPC11xx.h"

#include <io.h>

void disable_all_interrupts() {
  for (uint8_t i = WAKEUP0_IRQn; i <= EINT0_IRQn; i++) {
    //out_info_inthex("\r\nIRQn", i);
    NVIC_DisableIRQ(i);
  }
  __DSB();
  __ISB();
}

// input setter
void imu_handler_pid_set_state_PID(float __kp, float __ki, float __kd, float __angleLast, float __errorLast, float __errorSum);
void imu_handler_pid_set_state_INPUT(uint8_t __msg_flag, uint8_t __motor_on, float __angleTarget, uint32_t __pid_counter);
void imu_handler_pid_set_state_IMU(int16_t __accX, int16_t __accZ, int16_t __gyrY);

// target function
void imu_handler_pid_entry(uint8_t noyield, uint32_t pid_sampletime);
float __aeabi_fadd(float a, float b);

// from asm code
void _benchmark_timer_reset();
uint32_t _benchmark_timer_measure();
void _benchmark_helper_wait_1ms();
void _imu_handler_pid_entry_dummy(uint8_t noyield, uint32_t pid_sampletime);

// composite measurement primitive
uint32_t benchmark_measure(void (*fun_ptr)(uint8_t, uint32_t), uint8_t __noyield, uint32_t __pid_sampletime) {

  _benchmark_timer_reset();
  fun_ptr(__noyield, __pid_sampletime);

  uint32_t cycles = _benchmark_timer_measure();

  if (cycles > 0xFFFF) {
    out_error("unexpected cycle measurement");
    while(1); // out_error already blocks, but here we want to be sure
  }

  return cycles;
}
// quick and dirty adaption of "composite measurement primitive"
uint32_t benchmark_measure2(float (*fun_ptr)(float, float), float a, float b) {

  _benchmark_timer_reset();
  fun_ptr(a, b);

  uint32_t cycles = _benchmark_timer_measure();

  if (cycles > 0xFFFF) {
    out_error("unexpected cycle measurement");
    while(1); // out_error already blocks, but here we want to be sure
  }

  return cycles;
}


//#define USE_FIXED_BENCHMARK_INPUTS
// retrieve inputs from uart and set them
void set_inputs() {
#ifdef USE_FIXED_BENCHMARK_INPUTS
  //out_info("preparing");
  imu_handler_pid_set_state_PID(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  imu_handler_pid_set_state_INPUT(1, 0, 12.0f, 128);
  imu_handler_pid_set_state_IMU(1024, -1500, -2048);
#else
  out_info("wait4inputs");
  while (1) {
    int in_ch;
    uint32_t buf_ptr;

    // handle io
    while ((in_ch = in_handle()) == -3);

    if (in_ch == 100) {
      out_info("ok100");
      break;
    }

    switch (in_ch) {
      case -1:
        // nothing available
        break;
      case -2:
        // start sync error
        break;
      case 101:
        if (in_data_len != (4*(6) + 1*(2) + 1*(2) + 4*(2) + 2*(3) + 1*(2))) {
          out_info("nok101");
          break;
        }

        buf_ptr = (uint32_t)in_buffer + 4;
        float __kp = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        float __ki = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        float __kd = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        float __angleLast = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        float __errorLast = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        float __errorSum  = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        imu_handler_pid_set_state_PID(__kp, __ki, __kd, __angleLast, __errorLast, __errorSum);

        uint8_t __msg_flag = *((uint8_t*)(buf_ptr));
        buf_ptr += sizeof(uint8_t);
        uint8_t __motor_on = *((uint8_t*)(buf_ptr));
        buf_ptr += sizeof(uint8_t);
	// 2 byted padding
        buf_ptr += 2;
        float __angleTarget    = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        uint32_t __pid_counter = *((uint32_t*)(buf_ptr));
        buf_ptr += sizeof(uint32_t);
        imu_handler_pid_set_state_INPUT(__msg_flag, __motor_on, __angleTarget, __pid_counter);

        int16_t __accX = *((int16_t*)(buf_ptr));
        buf_ptr += sizeof(int16_t);
        int16_t __accZ = *((int16_t*)(buf_ptr));
        buf_ptr += sizeof(int16_t);
        int16_t __gyrY = *((int16_t*)(buf_ptr));
        buf_ptr += sizeof(int16_t);
        imu_handler_pid_set_state_IMU(__accX, __accZ, __gyrY);

        out_info("ok101");
        break;
      case 102:
        if (in_data_len != (4*(2))) {
          out_info("nok102");
          break;
        }

        buf_ptr = (uint32_t)in_buffer + 4;
        float a = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);
        float b = *((float*)(buf_ptr));
        buf_ptr += sizeof(float);

        uint32_t cycles_bl = benchmark_measure2((float (*)(float,float))_imu_handler_pid_entry_dummy, 0, 0);
        uint32_t cycles = benchmark_measure2(__aeabi_fadd, a, b);
        out_info_inthex("cyclesres", cycles - cycles_bl);
	float res = __aeabi_fadd(a, b);
        out_info_inthex("res", *((uint32_t*)&res));
        out_info("ok102");
        break;
      default:
        if (in_ch >= 0) {
          // unknown channel
          out_info_inthex("in_ch", in_ch);
          out_info("unknown channel");
          //out_error("unknown channel");
        } else {
          // some error
          out_info_inthex("in_ch", in_ch);
          out_info("unknown error");
          //out_error("unknown error");
        }
        break;
    }
  }
#endif
}


// one benchmark round (input-run-output)
void benchmark_run() {
  //out_info("\r\na benchmark");

  //out_info("disabling all interrupts (although they should be off at this point)");
  disable_all_interrupts();

  //out_info("baseline");
  uint32_t cycles_bl = benchmark_measure(_imu_handler_pid_entry_dummy, 0, 0);
  //out_info_inthex("cyclesbl", cycles_bl);

  // set the inputs (fixed or after retrieving them from the uart)
  //out_info_inthex("szfl", sizeof(float));
  set_inputs();

  //out_info("running");
  uint32_t cycles = benchmark_measure(imu_handler_pid_entry, 0, 0);
  out_info_inthex("cyclesres", cycles - cycles_bl);

  //while(1);
/*
  for (uint32_t i = 0; i < 1000 * 2; i++) {
    _benchmark_helper_wait_1ms();
  }
*/
}

#endif

