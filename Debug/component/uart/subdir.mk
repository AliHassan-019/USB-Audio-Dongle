################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/uart/fsl_adapter_lpuart.c 

C_DEPS += \
./component/uart/fsl_adapter_lpuart.d 

OBJS += \
./component/uart/fsl_adapter_lpuart.o 


# Each subdirectory must supply rules for building sources it contributes
component/uart/%.o: ../component/uart/%.c component/uart/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXC444VLH -DCPU_MCXC444VLH_cm0plus -DMCUXPRESSO_SDK -DUSB_STACK_BM -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DOSA_USED -DUSE_RTOS=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\source" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\drivers" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\CMSIS" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\CMSIS\m-profile" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\device" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\device\periph2" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\utilities" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\lists" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\utilities\str" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\utilities\debug_console_lite" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\uart" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\osa\config" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\osa" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\usb\include" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\usb\device" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\board" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\source\config\device\khci" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-uart

clean-component-2f-uart:
	-$(RM) ./component/uart/fsl_adapter_lpuart.d ./component/uart/fsl_adapter_lpuart.o

.PHONY: clean-component-2f-uart

