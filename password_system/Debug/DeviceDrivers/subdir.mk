################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DeviceDrivers/buzzer_driver.c 

OBJS += \
./DeviceDrivers/buzzer_driver.o 

C_DEPS += \
./DeviceDrivers/buzzer_driver.d 


# Each subdirectory must supply rules for building sources it contributes
DeviceDrivers/%.o DeviceDrivers/%.su DeviceDrivers/%.cyclo: ../DeviceDrivers/%.c DeviceDrivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../BSP/Inc -I../DeviceDrivers/Inc -I../Assets/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DeviceDrivers

clean-DeviceDrivers:
	-$(RM) ./DeviceDrivers/buzzer_driver.cyclo ./DeviceDrivers/buzzer_driver.d ./DeviceDrivers/buzzer_driver.o ./DeviceDrivers/buzzer_driver.su

.PHONY: clean-DeviceDrivers

