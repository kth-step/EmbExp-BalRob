
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

  if (cycles > 0xFFFFFF) {
    out_error("unexpected cycle measurement");
    while(1);
  }

  return cycles;
}


void benchmark_run() {
  out_info("\r\na benchmark");

  //out_info("disabling all interrupts (although they should be off at this point)");
  disable_all_interrupts();

  //out_info("baseline");
  uint32_t cycles_bl = benchmark_measure(_imu_handler_pid_entry_dummy, 0, 0);
  out_info_inthex("cyclesbl", cycles_bl);

  //out_info("preparing");
  imu_handler_pid_set_state_PID(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  imu_handler_pid_set_state_INPUT(1, 0, 12.0f, 128);
  imu_handler_pid_set_state_IMU(1024, -1500, -2048);

  //out_info("running");
  uint32_t cycles = benchmark_measure(imu_handler_pid_entry, 0, 0);
  out_info_inthex("cyclesres", cycles - cycles_bl);

  //while(1);
  for (uint32_t i = 0; i < 1000 * 2; i++) {
    _benchmark_helper_wait_1ms();
  }
}

#endif

