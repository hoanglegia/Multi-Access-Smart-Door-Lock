################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/Src/bsp_buzzer.c \
../BSP/Src/bsp_ssd1306.c \
../BSP/Src/bsp_system.c 

OBJS += \
./BSP/Src/bsp_buzzer.o \
./BSP/Src/bsp_ssd1306.o \
./BSP/Src/bsp_system.o 

C_DEPS += \
./BSP/Src/bsp_buzzer.d \
./BSP/Src/bsp_ssd1306.d \
./BSP/Src/bsp_system.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/Src/%.o BSP/Src/%.su BSP/Src/%.cyclo: ../BSP/Src/%.c BSP/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../BSP/Inc -I../DeviceDrivers/Inc -I../Assets/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-Src

clean-BSP-2f-Src:
	-$(RM) ./BSP/Src/bsp_buzzer.cyclo ./BSP/Src/bsp_buzzer.d ./BSP/Src/bsp_buzzer.o ./BSP/Src/bsp_buzzer.su ./BSP/Src/bsp_ssd1306.cyclo ./BSP/Src/bsp_ssd1306.d ./BSP/Src/bsp_ssd1306.o ./BSP/Src/bsp_ssd1306.su ./BSP/Src/bsp_system.cyclo ./BSP/Src/bsp_system.d ./BSP/Src/bsp_system.o ./BSP/Src/bsp_system.su

.PHONY: clean-BSP-2f-Src

