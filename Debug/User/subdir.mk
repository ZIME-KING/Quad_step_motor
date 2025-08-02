################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/software_Time.c \
../User/user_function.c \
../User/user_init.c 

OBJS += \
./User/software_Time.o \
./User/user_function.o \
./User/user_init.o 

C_DEPS += \
./User/software_Time.d \
./User/user_function.d \
./User/user_init.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o User/%.su User/%.cyclo: ../User/%.c User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User" -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User/UART" -I"C:/Users/king/Desktop/STM32CubeIDEworkspace_1.8.0/Quad_step_motor/User/SYS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User

clean-User:
	-$(RM) ./User/software_Time.cyclo ./User/software_Time.d ./User/software_Time.o ./User/software_Time.su ./User/user_function.cyclo ./User/user_function.d ./User/user_function.o ./User/user_function.su ./User/user_init.cyclo ./User/user_init.d ./User/user_init.o ./User/user_init.su

.PHONY: clean-User

