################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/UART/uart.c 

OBJS += \
./User/UART/uart.o 

C_DEPS += \
./User/UART/uart.d 


# Each subdirectory must supply rules for building sources it contributes
User/UART/%.o User/UART/%.su User/UART/%.cyclo: ../User/UART/%.c User/UART/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User" -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User/UART" -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User/SYS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-UART

clean-User-2f-UART:
	-$(RM) ./User/UART/uart.cyclo ./User/UART/uart.d ./User/UART/uart.o ./User/UART/uart.su

.PHONY: clean-User-2f-UART

