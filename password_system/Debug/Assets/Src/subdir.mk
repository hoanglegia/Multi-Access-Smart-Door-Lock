################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Assets/Src/ssd1306_fonts.c 

OBJS += \
./Assets/Src/ssd1306_fonts.o 

C_DEPS += \
./Assets/Src/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Assets/Src/%.o Assets/Src/%.su Assets/Src/%.cyclo: ../Assets/Src/%.c Assets/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../BSP/Inc -I../DeviceDrivers/Inc -I../Assets/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Assets-2f-Src

clean-Assets-2f-Src:
	-$(RM) ./Assets/Src/ssd1306_fonts.cyclo ./Assets/Src/ssd1306_fonts.d ./Assets/Src/ssd1306_fonts.o ./Assets/Src/ssd1306_fonts.su

.PHONY: clean-Assets-2f-Src

