################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_debug_console.c \
../utilities/fsl_notifier.c \
../utilities/fsl_shell.c 

OBJS += \
./utilities/fsl_debug_console.o \
./utilities/fsl_notifier.o \
./utilities/fsl_shell.o 

C_DEPS += \
./utilities/fsl_debug_console.d \
./utilities/fsl_notifier.d \
./utilities/fsl_shell.d 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=0 -D__USE_CMSIS -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=1 -D__MCUXPRESSO -DDEBUG -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1\board" -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1\source" -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1" -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1\drivers" -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1\utilities" -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1\startup" -I"C:\Users\Nahuel\Documents\MCUXpressoIDE_10.1.0_589\workspace\pendulum_control_v1\CMSIS" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


