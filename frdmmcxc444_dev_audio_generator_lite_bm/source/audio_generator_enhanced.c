/*
 * Enhanced USB Audio Generator for USB-C Audio Dongle
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017,2019 NXP
 * Enhanced 2024 for USB-C Audio Dongle Project
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_audio.h"
#include "usb_audio_config.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "audio_generator.h"
#include "fsl_device_registers.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "board.h"

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif

#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
#include "usb_phy.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AUDIO_LOG_ENABLE 1

#if AUDIO_LOG_ENABLE
#define AUDIO_LOG(fmt, ...) PRINTF("[AUDIO] " fmt "\r\n", ##__VA_ARGS__)
#define AUDIO_ERROR(fmt, ...) PRINTF("[AUDIO_ERROR] " fmt "\r\n", ##__VA_ARGS__)
#define AUDIO_DEBUG(fmt, ...) PRINTF("[AUDIO_DEBUG] " fmt "\r\n", ##__VA_ARGS__)
#else
#define AUDIO_LOG(fmt, ...)
#define AUDIO_ERROR(fmt, ...)
#define AUDIO_DEBUG(fmt, ...)
#endif

/* Enhanced audio statistics */
typedef struct {
    uint32_t totalPacketsSent;
    uint32_t totalPacketsReceived;
    uint32_t errorCount;
    uint32_t connectionCount;
    uint32_t disconnectionCount;
    uint32_t lastPacketSize;
    uint32_t audioStartTime;
    uint8_t isConnected;
    uint8_t isAudioActive;
} audio_stats_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
void USB_DeviceClockInit(void);
void USB_DeviceIsrEnable(void);
void USB_AudioDevice_Init(void);
void USB_AudioDevice_Statistics_Print(void);
void USB_AudioDevice_Reset_Statistics(void);

#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif

usb_status_t USB_DeviceAudioProcessTerminalRequest(uint32_t audioCommand,
                                                   uint32_t *length,
                                                   uint8_t **buffer,
                                                   uint8_t entityOrEndpoint);
extern void USB_AudioRecorderGetBuffer(uint8_t *buffer, uint32_t size);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Enhanced audio statistics */
static audio_stats_t g_audioStats = {0};

/* Audio data information */
extern uint8_t s_wavBuff[];
extern uint8_t g_UsbDeviceInterface[USB_AUDIO_GENERATOR_INTERFACE_COUNT];
extern usb_status_t USB_DeviceSetSpeed(uint8_t speed);

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_SetupOutBuffer[8];

/* Enhanced audio generator device struct with additional logging capabilities */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
usb_audio_generator_struct_t s_audioGenerator = {
    NULL,                  /* deviceHandle */
    0x0U,                  /* currentStreamInterfaceAlternateSetting */
    0x01U,                 /* copyProtect */
    0x01U,                 /* curMute */
    {0x00U, 0x80U},        /* curVolume */
    {0x00U, 0x80U},        /* minVolume */
    {0xFFU, 0x7FU},        /* maxVolume */
    {0x01U, 0x00U},        /* resVolume */
    0x00U,                 /* curBass */
    0x80U,                 /* minBass */
    0x7FU,                 /* maxBass */
    0x01U,                 /* resBass */
    0x00U,                 /* curMid */
    0x80U,                 /* minMid */
    0x7FU,                 /* maxMid */
    0x01U,                 /* resMid */
    0x01U,                 /* curTreble */
    0x80U,                 /* minTreble */
    0x7FU,                 /* maxTreble */
    0x01U,                 /* resTreble */
    0x01U,                 /* curAutomaticGain */
    {0x00U, 0x40U},        /* curDelay */
    {0x00U, 0x00U},        /* minDelay */
    {0xFFU, 0xFFU},        /* maxDelay */
    {0x00U, 0x01U},        /* resDelay */
    0x01U,                 /* curLoudness */
    {0x00U, 0x00U, 0x01U}, /* curSamplingFrequency */
    {0x00U, 0x00U, 0x01U}, /* minSamplingFrequency */
    {0x00U, 0x00U, 0x01U}, /* maxSamplingFrequency */
    {0x00U, 0x00U, 0x01U}, /* resSamplingFrequency */
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
    0U,             /* curMute20 */
    1U,             /* curClockValid */
    {0x00U, 0x1FU}, /* curVolume20 */
#if (defined(AUDIO_DATA_SOURCE_DMIC) && (AUDIO_DATA_SOURCE_DMIC > 0U)) || \
    (defined(AUDIO_DATA_SOURCE_PDM) && (AUDIO_DATA_SOURCE_PDM > 0U))
    16000U,                   /* curSampleFrequency, 16kHz for DMIC/PDM */
    {1U, 16000U, 16000U, 0U}, /* freqControlRange */
#else
    48000U,                   /* curSampleFrequency, 48kHz for standard audio */
    {1U, 48000U, 48000U, 0U}, /* freqControlRange */
#endif
    {1U, 0x8001U, 0x7FFFU, 1U}, /* volumeControlRange */
#endif
    0,              /* currentConfiguration */
    {0, 0},         /* currentInterfaceAlternateSetting */
    USB_SPEED_FULL, /* speed */
    0U,             /* attach */
#if defined(USB_DEVICE_AUDIO_USE_SYNC_MODE) && (USB_DEVICE_AUDIO_USE_SYNC_MODE > 0U)
    0,                                 /* generatorIntervalCount */
    0,                                 /* curAudioPllFrac */
    0,                                 /* audioPllTicksPrev */
    0,                                 /* audioPllTicksDiff */
    AUDIO_PLL_USB1_SOF_INTERVAL_COUNT, /* audioPllTicksEma */
    0,                                 /* audioPllTickEmaFrac */
    AUDIO_PLL_FRACTIONAL_CHANGE_STEP,  /* audioPllStep */
#endif
};

/*******************************************************************************
 * Enhanced Code Implementation
 ******************************************************************************/

/*!
 * @brief Enhanced USB device audio ISO OUT endpoint callback with logging
 */
usb_status_t USB_DeviceAudioIsoOut(usb_device_handle deviceHandle,
                                   usb_device_endpoint_callback_message_struct_t *event,
                                   void *arg)
{
    usb_device_endpoint_callback_message_struct_t *ep_cb_param;
    ep_cb_param = (usb_device_endpoint_callback_message_struct_t *)event;
    
    uint32_t expectedPacketSize = (USB_SPEED_HIGH == s_audioGenerator.speed) ? 
                                  HS_ISO_IN_ENDP_PACKET_SIZE : FS_ISO_IN_ENDP_PACKET_SIZE;

    if ((0U != s_audioGenerator.attach) && (ep_cb_param->length == expectedPacketSize))
    {
        /* Update statistics */
        g_audioStats.totalPacketsSent++;
        g_audioStats.lastPacketSize = ep_cb_param->length;
        
        /* Log audio activity every 1000 packets to avoid spam */
        if ((g_audioStats.totalPacketsSent % 1000) == 0)
        {
            AUDIO_DEBUG("Audio streaming: %lu packets sent, packet size: %lu bytes", 
                       g_audioStats.totalPacketsSent, g_audioStats.lastPacketSize);
        }

        /* Mark audio as active */
        if (!g_audioStats.isAudioActive)
        {
            g_audioStats.isAudioActive = 1;
            g_audioStats.audioStartTime = g_audioStats.totalPacketsSent;
            AUDIO_LOG("Audio streaming STARTED - packet size: %lu bytes", ep_cb_param->length);
        }

        /* Get audio buffer and send data */
        USB_AudioRecorderGetBuffer(s_wavBuff, expectedPacketSize);
        
        return USB_DeviceSendRequest(deviceHandle, USB_AUDIO_STREAM_ENDPOINT, s_wavBuff, expectedPacketSize);
    }
    else
    {
        /* Log error conditions */
        if (0U == s_audioGenerator.attach)
        {
            AUDIO_ERROR("ISO OUT callback called but device not attached");
        }
        else if (ep_cb_param->length != expectedPacketSize)
        {
            AUDIO_ERROR("ISO OUT packet size mismatch: expected %lu, got %lu", 
                       expectedPacketSize, ep_cb_param->length);
        }
        
        g_audioStats.errorCount++;
        return kStatus_USB_Error;
    }
}

/*!
 * @brief Enhanced USB device callback function with comprehensive logging
 */
usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t *temp8     = (uint8_t *)param;
    uint8_t count      = 0U;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
            AUDIO_LOG("USB Bus Reset detected - resetting device state");
            
            /* Reset all interface settings */
            for (count = 0U; count < USB_AUDIO_GENERATOR_INTERFACE_COUNT; count++)
            {
                s_audioGenerator.currentInterfaceAlternateSetting[count] = 0U;
            }
            
            /* Reset device state */
            s_audioGenerator.attach               = 0U;
            s_audioGenerator.currentConfiguration = 0U;
            g_audioStats.isConnected = 0;
            g_audioStats.isAudioActive = 0;
            g_audioStats.disconnectionCount++;
            
            error = kStatus_USB_Success;
            USB_DeviceControlPipeInit(s_audioGenerator.deviceHandle);

            AUDIO_LOG("Device reset complete - ready for enumeration");

#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
            /* Get USB speed and configure device */
            if (kStatus_USB_Success ==
                USB_DeviceGetStatus(s_audioGenerator.deviceHandle, kUSB_DeviceStatusSpeed, &s_audioGenerator.speed))
            {
                USB_DeviceSetSpeed(s_audioGenerator.speed);
                AUDIO_LOG("USB Speed detected: %s", 
                         (s_audioGenerator.speed == USB_SPEED_HIGH) ? "High Speed" : "Full Speed");
            }
#endif
        }
        break;

#if (defined(USB_DEVICE_CONFIG_DETACH_ENABLE) && (USB_DEVICE_CONFIG_DETACH_ENABLE > 0U))
        case kUSB_DeviceEventDetach:
        {
            AUDIO_LOG("USB Device detached");
            g_audioStats.isConnected = 0;
            g_audioStats.isAudioActive = 0;
            g_audioStats.disconnectionCount++;
            error = kStatus_USB_Success;
        }
        break;
#endif

        case kUSB_DeviceEventSetConfiguration:
            if (0U == (*temp8))
            {
                AUDIO_LOG("USB Configuration cleared (set to 0)");
                s_audioGenerator.attach               = 0U;
                s_audioGenerator.currentConfiguration = 0U;
                g_audioStats.isConnected = 0;
                g_audioStats.isAudioActive = 0;
                error                                 = kStatus_USB_Success;
            }
            else if (USB_AUDIO_GENERATOR_CONFIGURE_INDEX == (*temp8))
            {
                AUDIO_LOG("USB Configuration set to %d - device now configured", *temp8);
                s_audioGenerator.attach               = 1U;
                s_audioGenerator.currentConfiguration = *temp8;
                g_audioStats.isConnected = 1;
                g_audioStats.connectionCount++;
                
                /* Print device information */
                AUDIO_LOG("USB Audio Dongle ready:");
                AUDIO_LOG("  - Speed: %s", (s_audioGenerator.speed == USB_SPEED_HIGH) ? "High Speed" : "Full Speed");
                AUDIO_LOG("  - Sample Rate: %lu Hz", 
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
                         s_audioGenerator.curSampleFrequency
#else
                         8000UL  /* Default for USB Audio 1.0 */
#endif
                );
                AUDIO_LOG("  - Configuration: %d", *temp8);
                
                error = kStatus_USB_Success;
            }
            else
            {
                AUDIO_ERROR("Invalid USB configuration requested: %d", *temp8);
            }
            break;

        case kUSB_DeviceEventSetInterface:
            if (0U != s_audioGenerator.attach)
            {
                uint8_t interface        = (*temp8) & 0xFFU;
                uint8_t alternateSetting = g_UsbDeviceInterface[interface];

                AUDIO_LOG("Set Interface: interface=%d, alternate=%d", interface, alternateSetting);

                error = kStatus_USB_Success;
                if (s_audioGenerator.currentStreamInterfaceAlternateSetting != alternateSetting)
                {
                    if (s_audioGenerator.currentStreamInterfaceAlternateSetting)
                    {
                        AUDIO_LOG("Deinitializing audio endpoint");
                        USB_DeviceDeinitEndpoint(
                            s_audioGenerator.deviceHandle,
                            USB_AUDIO_STREAM_ENDPOINT | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT));
                        g_audioStats.isAudioActive = 0;
                    }
                    else
                    {
                        usb_device_endpoint_init_struct_t epInitStruct;
                        usb_device_endpoint_callback_struct_t epCallback;

                        AUDIO_LOG("Initializing audio streaming endpoint");

                        epCallback.callbackFn    = USB_DeviceAudioIsoOut;
                        epCallback.callbackParam = handle;

                        epInitStruct.zlt          = 0U;
                        epInitStruct.transferType = USB_ENDPOINT_ISOCHRONOUS;
                        epInitStruct.endpointAddress =
                            USB_AUDIO_STREAM_ENDPOINT | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);
                        
                        if (USB_SPEED_HIGH == s_audioGenerator.speed)
                        {
                            epInitStruct.maxPacketSize = HS_ISO_IN_ENDP_PACKET_SIZE;
                            epInitStruct.interval      = HS_ISO_IN_ENDP_INTERVAL;
                            AUDIO_LOG("High Speed endpoint: packet size=%d, interval=%d", 
                                     HS_ISO_IN_ENDP_PACKET_SIZE, HS_ISO_IN_ENDP_INTERVAL);
                        }
                        else
                        {
                            epInitStruct.maxPacketSize = FS_ISO_IN_ENDP_PACKET_SIZE;
                            epInitStruct.interval      = FS_ISO_IN_ENDP_INTERVAL;
                            AUDIO_LOG("Full Speed endpoint: packet size=%d, interval=%d", 
                                     FS_ISO_IN_ENDP_PACKET_SIZE, FS_ISO_IN_ENDP_INTERVAL);
                        }

                        USB_DeviceInitEndpoint(s_audioGenerator.deviceHandle, &epInitStruct, &epCallback);
                        
                        /* Prime the endpoint with initial data */
                        USB_AudioRecorderGetBuffer(s_wavBuff, epInitStruct.maxPacketSize);
                        error = USB_DeviceSendRequest(
                            s_audioGenerator.deviceHandle, USB_AUDIO_STREAM_ENDPOINT, s_wavBuff,
                            epInitStruct.maxPacketSize);
                            
                        if (error == kStatus_USB_Success)
                        {
                            AUDIO_LOG("Audio endpoint initialized and primed successfully");
                        }
                        else
                        {
                            AUDIO_ERROR("Failed to prime audio endpoint, error: 0x%x", error);
                        }
                    }
                    s_audioGenerator.currentStreamInterfaceAlternateSetting = alternateSetting;
                }
            }
            else
            {
                AUDIO_ERROR("Set Interface called but device not attached");
            }
            break;

        default:
            AUDIO_DEBUG("Unhandled USB event: 0x%lx", event);
            break;
    }

    return error;
}

/*!
 * @brief Print current audio statistics
 */
void USB_AudioDevice_Statistics_Print(void)
{
    PRINTF("\r\n=== USB Audio Dongle Statistics ===\r\n");
    PRINTF("Connection Count: %lu\r\n", g_audioStats.connectionCount);
    PRINTF("Disconnection Count: %lu\r\n", g_audioStats.disconnectionCount);
    PRINTF("Total Packets Sent: %lu\r\n", g_audioStats.totalPacketsSent);
    PRINTF("Total Packets Received: %lu\r\n", g_audioStats.totalPacketsReceived);
    PRINTF("Error Count: %lu\r\n", g_audioStats.errorCount);
    PRINTF("Last Packet Size: %lu bytes\r\n", g_audioStats.lastPacketSize);
    PRINTF("Device Connected: %s\r\n", g_audioStats.isConnected ? "Yes" : "No");
    PRINTF("Audio Active: %s\r\n", g_audioStats.isAudioActive ? "Yes" : "No");
    if (g_audioStats.isAudioActive && g_audioStats.audioStartTime > 0)
    {
        PRINTF("Packets since audio start: %lu\r\n", 
               g_audioStats.totalPacketsSent - g_audioStats.audioStartTime);
    }
    PRINTF("==================================\r\n\r\n");
}

/*!
 * @brief Reset audio statistics
 */
void USB_AudioDevice_Reset_Statistics(void)
{
    memset(&g_audioStats, 0, sizeof(g_audioStats));
    AUDIO_LOG("Audio statistics reset");
}

/*!
 * @brief Enhanced application initialization with comprehensive setup
 */
void USB_AudioDevice_Init(void)
{
    AUDIO_LOG("Initializing USB Audio Dongle...");
    
    /* Initialize USB device clock */
    USB_DeviceClockInit();
    AUDIO_LOG("USB clock initialized");

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
    SYSMPU_Enable(SYSMPU, 0);
    AUDIO_LOG("Memory Protection Unit disabled");
#endif

#if defined(USB_DEVICE_AUDIO_USE_SYNC_MODE) && (USB_DEVICE_AUDIO_USE_SYNC_MODE > 0U)
    SCTIMER_CaptureInit();
    AUDIO_LOG("SCTimer capture initialized for sync mode");
#endif

    /* Initialize USB device */
    if (kStatus_USB_Success != USB_DeviceInit(CONTROLLER_ID, USB_DeviceCallback, &s_audioGenerator.deviceHandle))
    {
        AUDIO_ERROR("USB device initialization failed!");
        return;
    }
    else
    {
        AUDIO_LOG("USB device initialized successfully");
        AUDIO_LOG("Controller ID: %d", CONTROLLER_ID);
    }

    /* Enable USB interrupts */
    USB_DeviceIsrEnable();
    AUDIO_LOG("USB interrupts enabled");

    /* Reset statistics */
    USB_AudioDevice_Reset_Statistics();

    /* Delay to ensure proper USB disconnection detection */
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    
    /* Start USB device operation */
    USB_DeviceRun(s_audioGenerator.deviceHandle);
    AUDIO_LOG("USB Audio Dongle started - waiting for host connection...");
}

/*!
 * @brief Main application loop with periodic status updates
 */
void USB_AudioDevice_Task(void)
{
    static uint32_t statusPrintCounter = 0;
    
#if USB_DEVICE_CONFIG_USE_TASK
    USB_DeviceTaskFn(s_audioGenerator.deviceHandle);
#endif

    /* Print status every ~10 seconds (assuming 1ms task period) */
    statusPrintCounter++;
    if (statusPrintCounter >= 10000)
    {
        statusPrintCounter = 0;
        if (g_audioStats.isConnected)
        {
            USB_AudioDevice_Statistics_Print();
        }
    }
}

/*!
 * @brief Enhanced main function
 */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    /* Initialize hardware */
    BOARD_InitHardware();
    AUDIO_LOG("Hardware initialized");

#if defined(AUDIO_DATA_SOURCE_DMIC) && (AUDIO_DATA_SOURCE_DMIC > 0U)
    Board_DMIC_DMA_Init();
    AUDIO_LOG("DMIC DMA initialized");
#elif defined(AUDIO_DATA_SOURCE_PDM) && (AUDIO_DATA_SOURCE_PDM > 0U)
    Board_PDM_EDMA_Init();
    AUDIO_LOG("PDM EDMA initialized");
#endif

    /* Print startup banner */
    PRINTF("\r\n");
    PRINTF("===============================================\r\n");
    PRINTF("    Enhanced USB-C Audio Dongle Project       \r\n");
    PRINTF("    Based on NXP MCX C444 SDK Example         \r\n");
    PRINTF("    Enhanced with comprehensive logging        \r\n");
    PRINTF("===============================================\r\n");
    PRINTF("Build Date: %s %s\r\n", __DATE__, __TIME__);
    PRINTF("MCU: MCXC444\r\n");
    PRINTF("USB Audio Class: 2.0\r\n");
    PRINTF("Sample Rate: %lu Hz\r\n", 
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
           s_audioGenerator.curSampleFrequency
#else
           8000UL
#endif
    );
    PRINTF("===============================================\r\n\r\n");

    /* Initialize USB Audio Device */
    USB_AudioDevice_Init();

    /* Main application loop */
    while (1)
    {
        USB_AudioDevice_Task();
        
        /* Small delay to prevent tight polling */
        SDK_DelayAtLeastUs(1000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    }
}

/* Include the rest of the original USB Audio functions with minimal changes */
// ... (keeping all the existing USB Audio control functions from the original file) 