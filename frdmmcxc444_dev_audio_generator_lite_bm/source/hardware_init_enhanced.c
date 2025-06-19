/*
 * Enhanced Hardware Initialization for USB-C Audio Dongle
 * Copyright 2024 NXP
 * Enhanced for USB-C Audio Dongle Project
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "usb_device_audio.h"
#include "audio_generator.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define HW_LOG_ENABLE 1

#if HW_LOG_ENABLE
#define HW_LOG(fmt, ...) PRINTF("[HW_INIT] " fmt "\r\n", ##__VA_ARGS__)
#define HW_ERROR(fmt, ...) PRINTF("[HW_ERROR] " fmt "\r\n", ##__VA_ARGS__)
#else
#define HW_LOG(fmt, ...)
#define HW_ERROR(fmt, ...)
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern usb_audio_generator_struct_t s_audioGenerator;

/*******************************************************************************
 * Enhanced Hardware Initialization Code
 ******************************************************************************/

/*!
 * @brief Enhanced board hardware initialization with logging
 */
void BOARD_InitHardware(void)
{
    PRINTF("[HW_INIT] Starting hardware initialization...\r\n");
    
    /* Initialize pins */
    PRINTF("[HW_INIT] Initializing pins...\r\n");
    BOARD_InitPins();
    PRINTF("[HW_INIT] Pins initialized successfully\r\n");
    
    /* Initialize clocks */
    PRINTF("[HW_INIT] Initializing clocks...\r\n");
    BOARD_BootClockRUN();
    PRINTF("[HW_INIT] Clocks initialized - System clock: %lu Hz\r\n", SystemCoreClock);
    
    /* Initialize debug console */
    PRINTF("[HW_INIT] Initializing debug console...\r\n");
    BOARD_InitDebugConsole();
    PRINTF("[HW_INIT] Debug console initialized successfully\r\n");
    
    PRINTF("[HW_INIT] Hardware initialization complete\r\n");
}

/*!
 * @brief Enhanced USB interrupt handler
 */
void USB0_IRQHandler(void)
{
    if (s_audioGenerator.deviceHandle != NULL)
    {
        USB_DeviceKhciIsrFunction(s_audioGenerator.deviceHandle);
    }
}

/*!
 * @brief Enhanced USB device clock initialization
 */
void USB_DeviceClockInit(void)
{
    PRINTF("[HW_INIT] Initializing USB device clock...\r\n");
    
    SystemCoreClockUpdate();
    PRINTF("[HW_INIT] System core clock: %lu Hz\r\n", SystemCoreClock);
    
    CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcIrc48M, 48000000U);
    PRINTF("[HW_INIT] USB FS clock enabled: 48 MHz\r\n");
}

/*!
 * @brief Enhanced USB ISR enable
 */
void USB_DeviceIsrEnable(void)
{
    uint8_t irqNumber;
    uint8_t usbDeviceKhciIrq[] = USB_IRQS;
    
    PRINTF("[HW_INIT] Configuring USB interrupt...\r\n");
    
    irqNumber = usbDeviceKhciIrq[CONTROLLER_ID - kUSB_ControllerKhci0];
    
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
    EnableIRQ((IRQn_Type)irqNumber);
    
    PRINTF("[HW_INIT] USB interrupt enabled (IRQ %d, Priority %d)\r\n", 
           irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
}

#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle)
{
    if (deviceHandle != NULL)
    {
        USB_DeviceKhciTaskFunction(deviceHandle);
    }
}
#endif

/*!
 * @brief Print hardware configuration information
 */
void USB_PrintHardwareInfo(void)
{
    PRINTF("\r\n=== Hardware Configuration ===\r\n");
    PRINTF("MCU: MCXC444\r\n");
    PRINTF("System Clock: %lu Hz\r\n", SystemCoreClock);
    PRINTF("USB Controller: KHCI\r\n");
    PRINTF("USB Speed: Full Speed (12 Mbps)\r\n");
    PRINTF("USB Audio Class: 2.0\r\n");
    PRINTF("Interrupt Priority: %d\r\n", USB_DEVICE_INTERRUPT_PRIORITY);
    PRINTF("==============================\r\n\r\n");
}

/*!
 * @brief Enhanced hardware verification function
 */
bool USB_VerifyHardwareConfiguration(void)
{
    bool configOk = true;
    
    HW_LOG("Verifying hardware configuration...");
    
    /* Check system clock */
    if (SystemCoreClock == 0)
    {
        HW_ERROR("System clock not initialized properly");
        configOk = false;
    }
    else
    {
        HW_LOG("System clock OK: %lu Hz", SystemCoreClock);
    }
    
    /* Verify USB controller configuration */
    #if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0U)
        HW_LOG("USB KHCI controller configured");
    #else
        HW_ERROR("USB KHCI controller not configured");
        configOk = false;
    #endif
    
    /* Check audio configuration */
    #if defined(USB_DEVICE_CONFIG_AUDIO) && (USB_DEVICE_CONFIG_AUDIO > 0U)
        HW_LOG("USB Audio class enabled");
    #else
        HW_ERROR("USB Audio class not enabled");
        configOk = false;
    #endif
    
    if (configOk)
    {
        HW_LOG("Hardware configuration verification PASSED");
    }
    else
    {
        HW_ERROR("Hardware configuration verification FAILED");
    }
    
    return configOk;
} 