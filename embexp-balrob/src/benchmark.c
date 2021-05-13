
#ifdef __BENCHMARK_MODE

#include <stdint.h>

#include <io.h>

// input setter
void imu_handler_pid_set_state_PID(float __kp, float __ki, float __kd, float __angleLast, float __errorLast, float __errorSum);
void imu_handler_pid_set_state_INPUT(uint8_t __msg_flag, uint8_t __motor_on, float __angleTarget, uint32_t __pid_counter);
void imu_handler_pid_set_state_IMU(int16_t __accX, int16_t __accZ, int16_t __gyrY);
// target function
void imu_handler_pid_entry(uint8_t noyield, uint32_t pid_sampletime);


// from asm code
void _benchmark_timer_reset();
uint32_t _benchmark_timer_measure();

uint32_t benchmark_measure(uint8_t __noyield, uint32_t __pid_sampletime) {

  _benchmark_timer_reset();
  imu_handler_pid_entry(__noyield, __pid_sampletime);

  uint32_t cycles = _benchmark_timer_measure();

  return cycles;
}


void benchmark_run() {
  out_info("\r\npreparing a benchmark");
  imu_handler_pid_set_state_PID(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  imu_handler_pid_set_state_INPUT(1, 0, 12.0f, 128);
  imu_handler_pid_set_state_IMU(1024, -1024, -2048);

  out_info("running a benchmark");
  uint32_t cycles = benchmark_measure(0, 0);

  out_info_inthex("cycles", cycles);

  //while(1);
  for (uint32_t i = 0; i < 1000 * 100; i++);
}

#endif

