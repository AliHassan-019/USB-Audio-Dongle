/*
 * Enhanced USB Device Descriptors for USB-C Audio Dongle
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Enhanced 2024 for USB-C Audio Dongle Project
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_DESCRIPTOR_ENHANCED_H__
#define __USB_DEVICE_DESCRIPTOR_ENHANCED_H__ 1

/*******************************************************************************
 * Enhanced Definitions for USB-C Audio Dongle
 ******************************************************************************/

/* Enhanced USB device configuration */
#define USB_DEVICE_VID (0x1FC9U)                    /* NXP Vendor ID */
#define USB_DEVICE_PID (0x00A5U)                    /* Enhanced Audio Dongle PID */
#define USB_DEVICE_DEMO_BCD_VERSION (0x0200U)       /* Version 2.0 */
#define USB_DEVICE_MAX_POWER (0x32U)                /* 100mA max power */

/* Enhanced audio interface configuration */
#define USB_AUDIO_GENERATOR_INTERFACE_COUNT (2U)
#define USB_AUDIO_GENERATOR_CONFIGURE_INDEX (1U)

/* Enhanced endpoint configuration for better audio quality */
#define USB_AUDIO_STREAM_ENDPOINT (0x02U)           /* Endpoint 2 for audio stream */
#define USB_AUDIO_CONTROL_ENDPOINT (0x81U)          /* Endpoint 1 IN for control */

/* Enhanced packet sizes for high-quality audio */
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
    /* USB Audio 2.0 enhanced packet sizes */
    #define FS_ISO_IN_ENDP_PACKET_SIZE (96U)        /* 48kHz * 2 bytes * 1ms */
    #define HS_ISO_IN_ENDP_PACKET_SIZE (384U)       /* 48kHz * 2 bytes * 4 samples/uframe */
    #define FS_ISO_IN_ENDP_INTERVAL (1U)            /* 1ms interval */
    #define HS_ISO_IN_ENDP_INTERVAL (4U)            /* 125us * 4 = 500us interval */
#else
    /* USB Audio 1.0 packet sizes */
    #define FS_ISO_IN_ENDP_PACKET_SIZE (32U)        /* 8kHz * 2 bytes * 2ms */
    #define HS_ISO_IN_ENDP_PACKET_SIZE (64U)        /* Higher quality for HS */
    #define FS_ISO_IN_ENDP_INTERVAL (1U)
    #define HS_ISO_IN_ENDP_INTERVAL (1U)
#endif

/* Enhanced control interface */
#define USB_AUDIO_CONTROL_INTERFACE_INDEX (0U)
#define USB_AUDIO_STREAM_INTERFACE_INDEX (1U)

/* Enhanced interface alternate settings */
#define USB_AUDIO_GENERATOR_CONTROL_INTERFACE_ALTERNATE_0 (0U)
#define USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_0 (0U)
#define USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_1 (1U)

/* Enhanced audio format configuration */
#define AUDIO_ENDPOINT_PACKET_SIZE FS_ISO_IN_ENDP_PACKET_SIZE
#define AUDIO_FORMAT_CHANNELS (2U)                   /* Stereo */
#define AUDIO_FORMAT_BITS (16U)                      /* 16-bit audio */
#define AUDIO_FORMAT_SAMPLE_RATE (48000U)            /* 48kHz sample rate */

/* Enhanced control interface interrupt endpoint */
#define FS_INTERRUPT_IN_PACKET_SIZE (8U)
#define FS_INTERRUPT_IN_INTERVAL (4U)               /* 4ms interval */

/* Enhanced Audio Control Unit IDs */
#define USB_AUDIO_RECORDER_CONTROL_INPUT_TERMINAL_ID (0x01U)
#define USB_AUDIO_RECORDER_CONTROL_FEATURE_UNIT_ID (0x02U)
#define USB_AUDIO_RECORDER_CONTROL_OUTPUT_TERMINAL_ID (0x03U)

#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
/* Audio 2.0 Clock Source Entity */
#define USB_AUDIO_RECORDER_CONTROL_CLOCK_SOURCE_ENTITY_ID (0x04U)
#endif

/* Enhanced String Descriptor Indices */
#define USB_STRING_DESCRIPTOR_MANUFACTURER (1U)      /* "NXP Semiconductors" */
#define USB_STRING_DESCRIPTOR_PRODUCT (2U)           /* "USB-C Audio Dongle Enhanced" */
#define USB_STRING_DESCRIPTOR_SERIAL (3U)            /* Serial number */
#define USB_STRING_DESCRIPTOR_CONFIG (4U)            /* Configuration string */

/* Enhanced Audio Class-Specific Descriptor Lengths */
#define USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH (0x09U)
#define USB_AUDIO_INPUT_TERMINAL_ONLY_DESC_SIZE (0x0CU)
#define USB_AUDIO_FEATURE_UNIT_ONLY_DESC_SIZE (0x09U)
#define USB_AUDIO_OUTPUT_TERMINAL_ONLY_DESC_SIZE (0x09U)
#define USB_AUDIO_STREAMING_IFACE_DESC_SIZE (0x07U)
#define USB_AUDIO_STREAMING_TYPE_I_DESC_SIZE (0x0BU)
#define USB_AUDIO_STREAMING_ENDP_DESC_SIZE (0x07U)
#define USB_ENDPOINT_AUDIO_DESCRIPTOR_LENGTH (0x09U)
#define USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH (0x07U)
#define USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH (0x08U)

/* Enhanced endpoint intervals */
#define ISO_IN_ENDP_INTERVAL (0x01U)

/*******************************************************************************
 * Enhanced API Functions
 ******************************************************************************/

/*!
 * @brief Enhanced USB device get descriptor function
 * Supports additional descriptors for enhanced functionality
 */
usb_status_t USB_DeviceGetDescriptor(usb_device_handle handle,
                                     usb_setup_struct_t *setup,
                                     uint32_t *length,
                                     uint8_t **buffer);

/*!
 * @brief Enhanced USB device set configuration
 * Includes validation and logging
 */
usb_status_t USB_DeviceSetConfigure(usb_device_handle handle, uint8_t configure);

/*!
 * @brief Enhanced USB device get configuration
 */
usb_status_t USB_DeviceGetConfigure(usb_device_handle handle, uint8_t *configure);

/*!
 * @brief Enhanced USB device set interface
 * Includes interface validation and audio endpoint management
 */
usb_status_t USB_DeviceSetInterface(usb_device_handle handle, uint8_t interface, uint8_t alternateSetting);

/*!
 * @brief Enhanced USB device get interface
 */
usb_status_t USB_DeviceGetInterface(usb_device_handle handle, uint8_t interface, uint8_t *alternateSetting);

/*!
 * @brief Enhanced USB device set speed
 * Optimizes configuration based on detected USB speed
 */
usb_status_t USB_DeviceSetSpeed(uint8_t speed);

/*!
 * @brief Get enhanced device information
 * Returns detailed device and audio configuration info
 */
void USB_DeviceGetEnhancedInfo(void);

/*******************************************************************************
 * Enhanced External Variables
 ******************************************************************************/
extern uint8_t g_UsbDeviceCurrentConfigure;
extern uint8_t g_UsbDeviceInterface[USB_AUDIO_GENERATOR_INTERFACE_COUNT];
extern uint8_t g_UsbDeviceDescriptor[];
extern uint8_t g_UsbDeviceConfigurationDescriptor[];
extern uint8_t g_UsbDeviceString0[];
extern uint8_t g_UsbDeviceString1[];
extern uint8_t g_UsbDeviceString2[];
extern uint8_t g_UsbDeviceString3[];
extern uint8_t g_UsbDeviceStringN[];

extern uint32_t g_UsbDeviceStringDescriptorLength[USB_DEVICE_STRING_COUNT];
extern uint8_t *g_UsbDeviceStringDescriptorArray[USB_DEVICE_STRING_COUNT];

extern usb_device_class_struct_t g_UsbDeviceAudioConfig;

/* Enhanced language list for multi-language support */
extern uint8_t g_UsbDeviceLanguageList[];

#endif /* __USB_DEVICE_DESCRIPTOR_ENHANCED_H__ */ 