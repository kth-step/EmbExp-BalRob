
#ifdef __BENCHMARK_MODE

#include <stdint.h>

#include <io.h>

// input setter
void imu_handler_pid_set_state_PID(float __kp, float __ki, float __kd, float __angleLast, float __errorLast, float __errorSum);
void imu_handler_pid_set_state_INPUT(uint8_t __msg_flag, uint8_t __motor_on, float __angleTarget, uint32_t __pid_counter);
void imu_handler_pid_set_state_IMU(int16_t __accX, int16_t __accZ, int16_t __gyrY);
// target function
void imu_handler_pid_entry(uint8_t noyield, uint32_t pid_sampletime);

void benchmark_run() {
  out_info("running a benchmark");

  while(1);
}

#endif

