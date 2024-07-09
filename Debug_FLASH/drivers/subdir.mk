################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/Driver_ADC.c \
../drivers/Driver_GPIO.c 

OBJS += \
./drivers/Driver_ADC.o \
./drivers/Driver_GPIO.o 

C_DEPS += \
./drivers/Driver_ADC.d \
./drivers/Driver_GPIO.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@drivers/Driver_ADC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


