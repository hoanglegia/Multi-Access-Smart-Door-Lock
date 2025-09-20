################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/buzzer_app.c \
../App/Src/ssd1306_app.c 

OBJS += \
./App/Src/buzzer_app.o \
./App/Src/ssd1306_app.o 

C_DEPS += \
./App/Src/buzzer_app.d \
./App/Src/ssd1306_app.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su App/Src/%.cyclo: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../BSP/Inc -I../DeviceDrivers/Inc -I../Assets/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/buzzer_app.cyclo ./App/Src/buzzer_app.d ./App/Src/buzzer_app.o ./App/Src/buzzer_app.su ./App/Src/ssd1306_app.cyclo ./App/Src/ssd1306_app.d ./App/Src/ssd1306_app.o ./App/Src/ssd1306_app.su

.PHONY: clean-App-2f-Src

