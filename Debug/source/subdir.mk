################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/audio_data.c \
../source/audio_generator.c \
../source/hardware_init.c \
../source/mtb.c \
../source/pin_mux.c \
../source/semihost_hardfault.c \
../source/usb_device_audio.c \
../source/usb_device_ch9.c \
../source/usb_device_descriptor.c 

C_DEPS += \
./source/audio_data.d \
./source/audio_generator.d \
./source/hardware_init.d \
./source/mtb.d \
./source/pin_mux.d \
./source/semihost_hardfault.d \
./source/usb_device_audio.d \
./source/usb_device_ch9.d \
./source/usb_device_descriptor.d 

OBJS += \
./source/audio_data.o \
./source/audio_generator.o \
./source/hardware_init.o \
./source/mtb.o \
./source/pin_mux.o \
./source/semihost_hardfault.o \
./source/usb_device_audio.o \
./source/usb_device_ch9.o \
./source/usb_device_descriptor.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXC444VLH -DCPU_MCXC444VLH_cm0plus -DMCUXPRESSO_SDK -DUSB_STACK_BM -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DOSA_USED -DUSE_RTOS=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\source" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\drivers" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\CMSIS" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\CMSIS\m-profile" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\device" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\device\periph2" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\utilities" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\lists" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\utilities\str" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\utilities\debug_console_lite" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\uart" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\osa\config" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\component\osa" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\usb\include" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\usb\device" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\board" -I"C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\source\config\device\khci" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\Artak\Documents\MCUXpressoIDE_24.12.148\workspace\USB Dongle_dev_audio_generator_lite_bm\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/audio_data.d ./source/audio_data.o ./source/audio_generator.d ./source/audio_generator.o ./source/hardware_init.d ./source/hardware_init.o ./source/mtb.d ./source/mtb.o ./source/pin_mux.d ./source/pin_mux.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/usb_device_audio.d ./source/usb_device_audio.o ./source/usb_device_ch9.d ./source/usb_device_ch9.o ./source/usb_device_descriptor.d ./source/usb_device_descriptor.o

.PHONY: clean-source

