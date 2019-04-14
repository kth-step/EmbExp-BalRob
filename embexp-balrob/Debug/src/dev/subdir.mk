################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dev/hw.c \
../src/dev/motor.c \
../src/dev/uart.c \
../src/dev/ui.c 

OBJS += \
./src/dev/hw.o \
./src/dev/motor.o \
./src/dev/uart.o \
./src/dev/ui.o 

C_DEPS += \
./src/dev/hw.d \
./src/dev/motor.d \
./src/dev/uart.d \
./src/dev/ui.d 


# Each subdirectory must supply rules for building sources it contributes
src/dev/%.o: ../src/dev/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11xx -D__LPC11XX__ -D__NEWLIB__ -I"C:\Users\andreas\Documents\LPCXpresso_8.2.2_650\workspace\embexp-balrob\inc" -I"C:\Users\andreas\Documents\LPCXpresso_8.2.2_650\workspace\CMSIS_CORE_LPC11xx\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__NEWLIB__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


