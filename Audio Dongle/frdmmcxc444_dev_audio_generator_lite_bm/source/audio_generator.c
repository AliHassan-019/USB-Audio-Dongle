/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor,
 * Copyright 2016 - 2017,2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <stdlib.h>
/*${standard_header_anchor}*/
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
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
#include "usb_phy.h"
#endif

/*******************************************************************************
 * If not defined in your descriptor headers, pull in the configure index
 ******************************************************************************/
#ifndef USB_AUDIO_GENERATOR_CONFIGURE_INDEX
#define USB_AUDIO_GENERATOR_CONFIGURE_INDEX (1U)
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
void USB_DeviceClockInit(void);
void USB_DeviceIsrEnable(void);
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif

#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
#if !((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
void USB_DeviceHsPhyChirpIssueWorkaround(void);
void USB_DeviceDisconnected(void);
#endif
#endif

usb_status_t USB_DeviceProcessClassRequest(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint32_t *length,
    uint8_t **buffer);

extern void USB_AudioRecorderGetBuffer(uint8_t *buffer, uint32_t size);
/* NEW: Speaker buffer handler */
extern void USB_AudioSpeakerBuffer(uint8_t *buffer, uint32_t size);

#if defined(AUDIO_DATA_SOURCE_DMIC) && (AUDIO_DATA_SOURCE_DMIC > 0U)
extern void Board_DMIC_DMA_Init(void);
#elif defined(AUDIO_DATA_SOURCE_PDM) && (AUDIO_DATA_SOURCE_PDM > 0U)
extern void Board_PDM_EDMA_Init(void);
#endif
#if defined(USB_DEVICE_AUDIO_USE_SYNC_MODE) && (USB_DEVICE_AUDIO_USE_SYNC_MODE > 0U)
extern void SCTIMER_CaptureInit(void);
#endif

/*******************************************************************************
 * Dummy mic‐in data buffer
 ******************************************************************************/
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_dummyMicData[FS_ISO_IN_ENDP_PACKET_SIZE];

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Audio data information */
extern uint8_t s_wavBuff[];

extern uint8_t g_UsbDeviceInterface[USB_AUDIO_GENERATOR_INTERFACE_COUNT];

extern usb_status_t USB_DeviceSetSpeed(uint8_t speed);

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_SetupOutBuffer[8];

/* Speaker streaming buffer (full-speed) */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_SpeakerOutBuffer[FS_ISO_OUT_ENDP_PACKET_SIZE];

/* Track speaker alternate setting */
static uint8_t s_speakerAlternateSetting = 0U;

/* Default value of audio generator device struct */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
usb_audio_generator_struct_t s_audioGenerator = {
    .deviceHandle                          = NULL,
    .currentStreamInterfaceAlternateSetting = 0U,
    .copyProtect                           = 1U,
    .curMute                               = 1U,
    .curVolume                             = {0x00U, 0x80U},
    .minVolume                             = {0x00U, 0x80U},
    .maxVolume                             = {0xFFU, 0x7FU},
    .resVolume                             = {0x01U, 0x00U},
    .curBass                               = 0U,
    .minBass                               = 0x80U,
    .maxBass                               = 0x7FU,
    .resBass                               = 0x01U,
    .curMid                                = 0U,
    .minMid                                = 0x80U,
    .maxMid                                = 0x7FU,
    .resMid                                = 0x01U,
    .curTreble                             = 1U,
    .minTreble                             = 0x80U,
    .maxTreble                             = 0x7FU,
    .resTreble                             = 0x01U,
    .curAutomaticGain                      = 1U,
    .curDelay                              = {0x00U, 0x40U},
    .minDelay                              = {0x00U, 0x00U},
    .maxDelay                              = {0xFFU, 0xFFU},
    .resDelay                              = {0x00U, 0x01U},
    .curLoudness                           = 1U,
    .curSamplingFrequency                  = {0x00U, 0x00U, 0x01U},
    .minSamplingFrequency                  = {0x00U, 0x00U, 0x01U},
    .maxSamplingFrequency                  = {0x00U, 0x00U, 0x01U},
    .resSamplingFrequency                  = {0x00U, 0x00U, 0x01U},
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
    .curMute20                             = 0U,
    .curClockValid                         = 1U,
    .curVolume20                           = {0x00U, 0x1FU},
#if (defined(AUDIO_DATA_SOURCE_DMIC)&&AUDIO_DATA_SOURCE_DMIC) || (defined(AUDIO_DATA_SOURCE_PDM)&&AUDIO_DATA_SOURCE_PDM)
    .curSampleFrequency                    = 16000U,
    .freqControlRange                      = {1U, 16000U, 16000U, 0U},
#else
    .curSampleFrequency                    = 8000U,
    .freqControlRange                      = {1U, 8000U, 8000U, 0U},
#endif
    .volumeControlRange                    = {1U, 0x8001U, 0x7FFFU, 1U},
#endif
    .currentConfiguration                  = 0U,
    .currentInterfaceAlternateSetting      = {0U, 0U},
    .speed                                 = USB_SPEED_FULL,
    .attach                                = 0U,
#if defined(USB_DEVICE_AUDIO_USE_SYNC_MODE)&&USB_DEVICE_AUDIO_USE_SYNC_MODE
    .generatorIntervalCount                = 0U,
    .curAudioPllFrac                       = 0U,
    .audioPllTicksPrev                     = 0U,
    .audioPllTicksDiff                     = 0U,
    .audioPllTicksEma                      = AUDIO_PLL_USB1_SOF_INTERVAL_COUNT,
    .audioPllTickEmaFrac                   = 0U,
    .audioPllStep                          = AUDIO_PLL_FRACTIONAL_CHANGE_STEP,
#endif
};

/*******************************************************************************
 * Helpers
 ******************************************************************************/
static void FillDummyMicData(void)
{
    static uint8_t toggle = 0x00;
    for (uint32_t i = 0; i < FS_ISO_IN_ENDP_PACKET_SIZE; ++i) {
        s_dummyMicData[i] = (toggle & 0x80) ? 0xAA : 0x55;
    }
    toggle += 0x20;
}

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief ISO IN callback for microphone.
 */
usb_status_t USB_DeviceAudioIsoIn(
    usb_device_handle deviceHandle,
    usb_device_endpoint_callback_message_struct_t *event,
    void *arg)
{
    if (s_audioGenerator.attach &&
        event->length == FS_ISO_IN_ENDP_PACKET_SIZE)
    {
        FillDummyMicData();
        PRINTF("[AUDIO] Mic IN packet sent (%d bytes).\r\n", FS_ISO_IN_ENDP_PACKET_SIZE);
        return USB_DeviceSendRequest(
            deviceHandle,
            USB_AUDIO_STREAM_ENDPOINT,
            s_dummyMicData,
            FS_ISO_IN_ENDP_PACKET_SIZE
        );
    }
    PRINTF("[ERROR] Mic IN packet error or device not attached.\r\n");
    return kStatus_USB_Error;
}

/*!
 * @brief ISO OUT callback for speaker.
 */
usb_status_t USB_DeviceAudioIsoOutSpeaker(
    usb_device_handle deviceHandle,
    usb_device_endpoint_callback_message_struct_t *event,
    void *arg)
{
    if (s_audioGenerator.attach &&
        event->length ==
        ((USB_SPEED_HIGH == s_audioGenerator.speed)
            ? HS_ISO_OUT_ENDP_PACKET_SIZE
            : FS_ISO_OUT_ENDP_PACKET_SIZE))
    {
        USB_AudioSpeakerBuffer(event->buffer, event->length);
        PRINTF("[AUDIO] Speaker OUT packet received (%d bytes).\r\n", event->length);
        return USB_DeviceRecvRequest(
            deviceHandle,
            USB_AUDIO_OUT_ENDPOINT,
            s_SpeakerOutBuffer,
            ((USB_SPEED_HIGH == s_audioGenerator.speed)
                ? HS_ISO_OUT_ENDP_PACKET_SIZE
                : FS_ISO_OUT_ENDP_PACKET_SIZE)
        );
    }
    PRINTF("[ERROR] Speaker OUT packet error or device not attached.\r\n");
    return kStatus_USB_Error;
}

/*!
 * @brief USB device callback (Control / Set configuration / Set interface).
 */
usb_status_t USB_DeviceCallback(
    usb_device_handle handle,
    uint32_t event,
    void *param)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t *temp8 = (uint8_t *)param;
    uint8_t count = 0U;

    switch (event) {
    case kUSB_DeviceEventBusReset:
        for (count = 0; count < USB_AUDIO_GENERATOR_INTERFACE_COUNT; ++count)
            s_audioGenerator.currentInterfaceAlternateSetting[count] = 0U;
        s_audioGenerator.attach = 0U;
        s_audioGenerator.currentConfiguration = 0U;
        error = kStatus_USB_Success;
        USB_DeviceControlPipeInit(s_audioGenerator.deviceHandle);
#if defined(USB_DEVICE_CONFIG_EHCI) || defined(USB_DEVICE_CONFIG_LPCIP3511HS)
        if (USB_DeviceGetStatus &&
            USB_DeviceGetStatus(s_audioGenerator.deviceHandle,
                                kUSB_DeviceStatusSpeed,
                                &s_audioGenerator.speed) == kStatus_USB_Success)
        {
            USB_DeviceSetSpeed(s_audioGenerator.speed);
        }
#endif
        PRINTF("[USB] Bus reset detected.\r\n");
        break;

    case kUSB_DeviceEventSetConfiguration:
        if (*temp8 == 0U) {
            s_audioGenerator.attach = 0U;
            s_audioGenerator.currentConfiguration = 0U;
            error = kStatus_USB_Success;
            PRINTF("[USB] Device detached (configuration 0).\r\n");
        } else if (*temp8 == USB_AUDIO_GENERATOR_CONFIGURE_INDEX) {
            s_audioGenerator.attach = 1U;
            s_audioGenerator.currentConfiguration = *temp8;
            error = kStatus_USB_Success;
            PRINTF("[USB] Device configured (configuration %d).\r\n", *temp8);
        }
        break;

    case kUSB_DeviceEventSetInterface:
        if (s_audioGenerator.attach) {
            uint8_t interface = *temp8;
            uint8_t alt      = g_UsbDeviceInterface[interface];
            error = kStatus_USB_Success;

            if (interface == USB_AUDIO_STREAM_INTERFACE_INDEX) {
                if (s_audioGenerator.currentStreamInterfaceAlternateSetting != alt) {
                    if (alt) {
                        USB_DeviceDeinitEndpoint(
                            handle,
                            USB_AUDIO_STREAM_ENDPOINT |
                            (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)
                        );
                        PRINTF("[AUDIO] Mic IN streaming stopped (alt %d).\r\n", alt);
                    } else {
                        usb_device_endpoint_init_struct_t    epInitStruct;
                        usb_device_endpoint_callback_struct_t epCallback;

                        epCallback.callbackFn    = USB_DeviceAudioIsoIn;
                        epCallback.callbackParam = handle;
                        epInitStruct.zlt          = 0U;
                        epInitStruct.transferType = USB_ENDPOINT_ISOCHRONOUS;
                        epInitStruct.endpointAddress =
                            USB_AUDIO_STREAM_ENDPOINT |
                            (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);
                        epInitStruct.maxPacketSize =
                            FS_ISO_IN_ENDP_PACKET_SIZE;
                        epInitStruct.interval =
                            FS_ISO_IN_ENDP_INTERVAL;

                        USB_DeviceInitEndpoint(handle, &epInitStruct, &epCallback);
                        /* Prime first IN transfer */
                        FillDummyMicData();
                        USB_DeviceSendRequest(
                            handle,
                            USB_AUDIO_STREAM_ENDPOINT,
                            s_dummyMicData,
                            FS_ISO_IN_ENDP_PACKET_SIZE
                        );
                        PRINTF("[AUDIO] Mic IN streaming started (alt %d).\r\n", alt);
                    }
                    s_audioGenerator.currentStreamInterfaceAlternateSetting = alt;
                }
            }
            else if (interface == USB_AUDIO_SPEAKER_STREAM_INTERFACE_INDEX) {
                if (s_speakerAlternateSetting != alt) {
                    if (alt) {
                        USB_DeviceDeinitEndpoint(handle, USB_AUDIO_OUT_ENDPOINT);
                        PRINTF("[AUDIO] Speaker OUT streaming stopped (alt %d).\r\n", alt);
                    } else {
                        usb_device_endpoint_init_struct_t    epInitStruct;
                        usb_device_endpoint_callback_struct_t epCallback;

                        epCallback.callbackFn    = USB_DeviceAudioIsoOutSpeaker;
                        epCallback.callbackParam = handle;
                        epInitStruct.zlt          = 0U;
                        epInitStruct.transferType = USB_ENDPOINT_ISOCHRONOUS;
                        epInitStruct.endpointAddress = USB_AUDIO_OUT_ENDPOINT;
                        epInitStruct.maxPacketSize =
                            FS_ISO_OUT_ENDP_PACKET_SIZE;
                        epInitStruct.interval = ISO_OUT_ENDP_INTERVAL;

                        USB_DeviceInitEndpoint(handle, &epInitStruct, &epCallback);
                        USB_DeviceRecvRequest(
                            handle,
                            USB_AUDIO_OUT_ENDPOINT,
                            s_SpeakerOutBuffer,
                            FS_ISO_OUT_ENDP_PACKET_SIZE
                        );
                        PRINTF("[AUDIO] Speaker OUT streaming started (alt %d).\r\n", alt);
                    }
                    s_speakerAlternateSetting = alt;
                }
            }
        }
        break;

    default:
        break;
    }

    return error;
}

/*!
 * @brief Application initialization.
 */
void APPInit(void)
{
    USB_DeviceClockInit();
#if defined(FSL_FEATURE_SOC_SYSMPU_COUNT)
    SYSMPU_Enable(SYSMPU, 0);
#endif
#if USB_DEVICE_AUDIO_USE_SYNC_MODE
    SCTIMER_CaptureInit();
#endif

    usb_status_t usbInitStatus = USB_DeviceInit(
            CONTROLLER_ID,
            USB_DeviceCallback,
            &s_audioGenerator.deviceHandle);
    PRINTF("[DEBUG] USB_DeviceInit() returned: %d\r\n", usbInitStatus);
    if (usbInitStatus != kStatus_USB_Success)
    {
        PRINTF("[ERROR] USB device failed to initialize!\r\n");
        return;
    } else {
        PRINTF("USB device audio generator demo\r\n");
    }

    USB_DeviceIsrEnable();
    PRINTF("[DEBUG] USB_DeviceIsrEnable() called.\r\n");
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    USB_DeviceRun(s_audioGenerator.deviceHandle);
    PRINTF("[DEBUG] USB_DeviceRun() called.\r\n");
}

#if defined(__CC_ARM)||defined(__ARMCC_VERSION)||defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    BOARD_InitHardware();
    BOARD_InitDebugConsole(); // Ensure debug console is initialized for COM port output
#if AUDIO_DATA_SOURCE_DMIC
    Board_DMIC_DMA_Init();
#elif AUDIO_DATA_SOURCE_PDM
    Board_PDM_EDMA_Init();
#endif
    APPInit();
    PRINTF("[INFO] Audio dongle firmware started.\r\n");
    while (1) {
#if USB_DEVICE_CONFIG_USE_TASK
        USB_DeviceTaskFn(s_audioGenerator.deviceHandle);
#endif
    }
}

/*!
 * @brief Stub for class-specific requests.
 */
usb_status_t USB_DeviceProcessClassRequest(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint32_t *length,
    uint8_t **buffer)
{
    (void)handle;
    (void)setup;
    (void)length;
    (void)buffer;
    return kStatus_USB_Success;
}
