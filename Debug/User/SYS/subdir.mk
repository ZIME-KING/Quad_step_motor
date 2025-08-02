################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/SYS/sys.c 

OBJS += \
./User/SYS/sys.o 

C_DEPS += \
./User/SYS/sys.d 


# Each subdirectory must supply rules for building sources it contributes
User/SYS/%.o User/SYS/%.su User/SYS/%.cyclo: ../User/SYS/%.c User/SYS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User" -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User/UART" -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User/SYS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-SYS

clean-User-2f-SYS:
	-$(RM) ./User/SYS/sys.cyclo ./User/SYS/sys.d ./User/SYS/sys.o ./User/SYS/sys.su

.PHONY: clean-User-2f-SYS

