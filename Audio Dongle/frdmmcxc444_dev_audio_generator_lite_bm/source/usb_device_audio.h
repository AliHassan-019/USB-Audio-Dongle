/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_AUDIO_H__
#define __USB_DEVICE_AUDIO_H__ 1

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#if USB_DEVICE_CONFIG_AUDIO

/*!
 * @addtogroup usb_device_audio_drv
 * @{
 */

/*!
 * @name USB Audio class codes
 * @{
 */

/*! @brief Audio device class code */
#define USB_DEVICE_CONFIG_AUDIO_CLASS_CODE (0x01U)

/*! @brief Audio device subclass codes */
#define USB_DEVICE_AUDIO_CONTROL_SUBCLASS (0x01U)
#define USB_DEVICE_AUDIO_STREAM_SUBCLASS  (0x02U)

/*! @brief Audio class-specific descriptor types */
#define USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE (0x24)
#define USB_DESCRIPTOR_TYPE_AUDIO_CS_ENDPOINT  (0x25)

/*! @brief Control interface descriptor sub-types */
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_HEADER          (0x01)
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_INPUT_TERMINAL  (0x02)
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_OUTPUT_TERMINAL (0x03)
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_FEATURE_UNIT    (0x06)
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_CLOCK_SOURCE_UNIT (0x0A)
#endif

/*! @brief Streaming interface descriptor sub-types */
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_AS_GENERAL  (0x01)
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_FORMAT_TYPE (0x02)

/*! @}*/

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Called by the USB stack to stall/un-stall an endpoint.
 *
 * @param handle USB device handle
 * @param ep     endpoint address
 * @param stall  1 = stall, 0 = unstall
 * @return kStatus_USB_Success
 */
usb_status_t USB_DeviceConfigureEndpointStatus(
    usb_device_handle handle,
    uint8_t ep,
    uint8_t stall);

/*!
 * @brief Called by the USB stack to enable/disable remote wakeup.
 *
 * @param handle USB device handle
 * @param enable 1 = enable, 0 = disable
 * @return kStatus_USB_Success
 */
usb_status_t USB_DeviceConfigureRemoteWakeup(
    usb_device_handle handle,
    uint8_t enable);

/*!
 * @brief Provides a buffer for class-specific OUT data (e.g. Set requests).
 *
 * @param handle USB device handle
 * @param setup  pointer to the setup packet
 * @param buffer[out] pointer to where to return buffer pointer
 * @param length[out] pointer to where to return buffer length
 * @return kStatus_USB_Success
 */
usb_status_t USB_DeviceGetClassReceiveBuffer(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint8_t **buffer,
    uint32_t *length);

/*!
 * @brief Provides the 8-byte setup-packet buffer.
 *
 * @param handle      USB device handle
 * @param setupBuffer[out] pointer to where to return the setup buffer ptr
 * @return kStatus_USB_Success
 */
usb_status_t USB_DeviceGetSetupBuffer(
    usb_device_handle handle,
    usb_setup_struct_t **setupBuffer);

/*!
 * @brief Handler for class-specific Audio requests on EP0.
 *
 * Called by the stack when it receives an Audio class request.
 *
 * @param handle USB device handle
 * @param setup  pointer to the setup packet
 * @param buffer[in/out] data buffer pointer
 * @param length[in/out] data length
 * @return status of the request
 */
usb_status_t USB_DeviceProcessClassRequest(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint32_t *length,
    uint8_t **buffer);

/*!
 * @brief Forwarder used by your descriptor code stub.
 *
 * This one simply calls into your audio-generator’s real
 * `USB_DeviceProcessClassRequest(…)`.
 */
usb_status_t USB_DeviceAudioClassRequest(
    usb_device_handle handle,
    usb_setup_struct_t *setup,
    uint8_t **buffer,
    uint32_t *length);

/*!
 * @brief Called by your ISO-OUT (speaker) callback to deliver PCM to the DAC,
 *        or drop it if you don’t support speaker.
 *
 * @param buffer pointer to audio data
 * @param size   length in bytes
 */
void USB_AudioSpeakerBuffer(uint8_t *buffer, uint32_t size);

/*! @}*/ /* end of usb_device_audio_drv */

#endif /* USB_DEVICE_CONFIG_AUDIO */

#endif /* __USB_DEVICE_AUDIO_H__ */
