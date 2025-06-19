/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#if USB_DEVICE_CONFIG_AUDIO
#include "usb_device_audio.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

/*!
 * @brief Dummy implementation of endpoint stall/un-stall.
 *
 * The USB stack will call this when it needs to stall or unstall an endpoint.
 */
usb_status_t USB_DeviceConfigureEndpointStatus(
    usb_device_handle handle,
    uint8_t ep,       /* endpoint address */
    uint8_t stall)    /* 1 = stall, 0 = unstall */
{
    /* nothing to do in this application */
    (void)handle; (void)ep; (void)stall;
    return kStatus_USB_Success;
}

/*!
 * @brief Dummy implementation of remote-wakeup config.
 */
usb_status_t USB_DeviceConfigureRemoteWakeup(
    usb_device_handle handle,
    uint8_t enable)
{
    /* nothing to do in this application */
    (void)handle; (void)enable;
    return kStatus_USB_Success;
}

/*!
 * @brief Provides a buffer for class-specific OUT data (e.g. Set requests).
 */
usb_status_t USB_DeviceGetClassReceiveBuffer(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint8_t **buffer,
    uint32_t *length)
{
    static uint8_t s_classRxBuf[64];
    *buffer = s_classRxBuf;
    *length = sizeof(s_classRxBuf);
    (void)handle;
    (void)setup;
    return kStatus_USB_Success;
}

/*!
 * @brief Provides the 8-byte setup packet buffer.
 */
usb_status_t USB_DeviceGetSetupBuffer(
    usb_device_handle handle,
    usb_setup_struct_t **setupBuffer)
{
    static usb_setup_struct_t s_setup;
    *setupBuffer = &s_setup;
    (void)handle;
    return kStatus_USB_Success;
}

/*!
 * @brief Forward class-specific requests into your audio generator.
 *
 * The stack will call this when it sees an Audio class request on EP0.
 */
usb_status_t USB_DeviceAudioClassRequest(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint8_t **buffer,
    uint32_t *length)
{
    /* audio_generator.c implements USB_DeviceProcessClassRequest(length, buffer) */
    return USB_DeviceProcessClassRequest(handle, setup, length, buffer);
}

/*!
 * @brief Stub for speaker data path.
 *
 * Called by your ISO-OUT speaker callback to push data
 * into your DAC or just drop it.
 */
void USB_AudioSpeakerBuffer(uint8_t *buffer, uint32_t size)
{
    /* TODO: send 'buffer' of length 'size' to your DAC,
       or just discard it if you don't need speaker support. */
    (void)buffer;
    (void)size;
}

#endif /* USB_DEVICE_CONFIG_AUDIO */
