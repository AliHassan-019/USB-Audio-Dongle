/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_audio.h"
#include "usb_audio_config.h"
#include "usb_device_descriptor.h"
#include "audio_generator.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t g_UsbDeviceCurrentConfigure = 0U;
uint8_t g_UsbDeviceInterface[USB_AUDIO_GENERATOR_INTERFACE_COUNT];

/* Define device descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_DEVICE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_DEVICE,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION),
    USB_DEVICE_CLASS,
    USB_DEVICE_SUBCLASS,
    USB_DEVICE_PROTOCOL,
    USB_CONTROL_MAX_PACKET_SIZE,
    USB_SHORT_GET_LOW(USB_DEVICE_VID),
    USB_SHORT_GET_HIGH(USB_DEVICE_VID),
    USB_SHORT_GET_LOW(USB_DEVICE_PID),
    USB_SHORT_GET_HIGH(USB_DEVICE_PID),
    USB_SHORT_GET_LOW(USB_DEVICE_DEMO_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_DEMO_BCD_VERSION),
    0x01U, /* Manufacturer String Index */
    0x02U, /* Product String Index      */
    0x00U, /* Serial Number String Index*/
    USB_DEVICE_CONFIGURATION_COUNT,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceConfigurationDescriptor[] = {
#if (USB_DEVICE_CONFIG_AUDIO_CLASS_2_0)
    /* ===== USB Audio 2.0 configuration ===== */

    /* Configuration Descriptor */
    USB_DESCRIPTOR_LENGTH_CONFIGURE,
    USB_DESCRIPTOR_TYPE_CONFIGURE,
    USB_SHORT_GET_LOW(
        USB_DESCRIPTOR_LENGTH_CONFIGURE + 0x08U          /* IAD */
        + USB_DESCRIPTOR_LENGTH_INTERFACE                /* AC IF */
        + USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH + 1U /* AC HEADER (add 1 for extra baInterfaceNr) */
        + 0x08U                                          /* Clock Source */
        + 0x11U                                          /* Input Terminal */
        + 0x0EU                                          /* Feature Unit */
        + 0x0CU                                          /* Output Terminal */
        + USB_DESCRIPTOR_LENGTH_INTERFACE                /* AS Mic Alt-0 */
        + USB_DESCRIPTOR_LENGTH_INTERFACE                /* AS Mic Alt-1 */
        + 0x10U                                          /* Mic AS_GENERAL */
        + 0x06U                                          /* Mic FORMAT_TYPE_I */
        + USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH /* Mic ISO IN EP */
        + USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH       /* Mic CS-ISO IN EP */
        + USB_DESCRIPTOR_LENGTH_INTERFACE                /* AS Speaker Alt-0 */
        + USB_DESCRIPTOR_LENGTH_INTERFACE                /* AS Speaker Alt-1 */
        + 0x10U                                          /* Speaker AS_GENERAL */
        + 0x06U                                          /* Speaker FORMAT_TYPE_I */
        + USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH /* Speaker ISO OUT EP */
        + USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH       /* Speaker CS-ISO OUT EP */
        ),
    USB_SHORT_GET_HIGH(
        USB_DESCRIPTOR_LENGTH_CONFIGURE + 0x08U + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH + 1U + 0x08U + 0x11U + 0x0EU + 0x0CU + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_INTERFACE + 0x10U + 0x06U + USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH + USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_INTERFACE + 0x10U + 0x06U + USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH + USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH),
    3U,                                  /* bNumInterfaces */
    USB_AUDIO_GENERATOR_CONFIGURE_INDEX, /* bConfigurationValue */
    0x00U,                               /* iConfiguration */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK)
#if defined(USB_DEVICE_CONFIG_SELF_POWER) && (USB_DEVICE_CONFIG_SELF_POWER > 0U)
        | (1U << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT)
#endif
#if defined(USB_DEVICE_CONFIG_REMOTE_WAKEUP) && (USB_DEVICE_CONFIG_REMOTE_WAKEUP > 0U)
        | (1U << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT)
#endif
        ,  /* bmAttributes */
    0xFAU, /* bMaxPower */

    /* Interface Association Descriptor */
    0x08U,
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    0x00U, /* bFirstInterface */
    0x03U, /* bInterfaceCount */
    USB_AUDIO_CLASS,
    0x00U,
    USB_AUDIO_PROTOCOL,
    0x00U, /* iFunction */

    /* Standard AC Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_CONTROL_INTERFACE_INDEX,
    USB_AUDIO_GENERATOR_CONTROL_INTERFACE_ALTERNATE_0,
    0x00U,
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOCONTROL,
    USB_AUDIO_PROTOCOL,
    0x02U, /* iInterface */

    /* Class-Specific AC Header Descriptor */
    USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH + 1U, // length +1 for extra baInterfaceNr
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_HEADER,
    0x00U,
    0x02U, /* bcdADC = 2.00 */
    0x03U, /* bCategory: Microphone */
    0x3DU, /* wTotalLength L (incremented by 1 for extra baInterfaceNr) */
    0x00U, /* wTotalLength H */
    0x02U, /* bInCollection = 2 */
    USB_AUDIO_STREAM_INTERFACE_INDEX,         /* baInterfaceNr(1) = 1 (mic) */
    USB_AUDIO_STREAM_INTERFACE_INDEX + 1,     /* baInterfaceNr(2) = 2 (speaker) */

    /* Clock Source Descriptor */
    0x08U,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_CLOCK_SOURCE_UNIT,
    USB_AUDIO_RECORDER_CONTROL_CLOCK_SOURCE_ENTITY_ID,
    0x01U,
    0x07U,
    0x00U,
    0x02U,

    /* Input Terminal Descriptor */
    0x11U,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_INPUT_TERMINAL,
    USB_AUDIO_RECORDER_CONTROL_INPUT_TERMINAL_ID,
    0x01U,
    0x02U,
    0x00U,
    USB_AUDIO_RECORDER_CONTROL_CLOCK_SOURCE_ENTITY_ID,
    0x01U,
    0x01U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x02U,

    /* Feature Unit Descriptor */
    0x0EU,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_FEATURE_UNIT,
    USB_AUDIO_RECORDER_CONTROL_FEATURE_UNIT_ID,
    USB_AUDIO_RECORDER_CONTROL_INPUT_TERMINAL_ID,
    0x0FU,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,

    /* Output Terminal Descriptor */
    0x0CU,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_OUTPUT_TERMINAL,
    USB_AUDIO_RECORDER_CONTROL_OUTPUT_TERMINAL_ID,
    0x01U,
    0x01U,
    0x00U,
    USB_AUDIO_RECORDER_CONTROL_FEATURE_UNIT_ID,
    USB_AUDIO_RECORDER_CONTROL_CLOCK_SOURCE_ENTITY_ID,
    0x00U,
    0x00U,
    0x00U,

    /* --- Microphone Streaming (AS) Alt-0 --- */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_STREAM_INTERFACE_INDEX,
    USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_0,
    0x00U,
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOSTREAM,
    USB_AUDIO_PROTOCOL,
    0x02U,

    /* --- Microphone Streaming (AS) Alt-1 --- */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_STREAM_INTERFACE_INDEX,
    USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_1,
    0x01U,
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOSTREAM,
    USB_AUDIO_PROTOCOL,
    0x02U,

    /* Class-Specific AS_GENERAL (Mic) */
    0x10U,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_AS_GENERAL,
    USB_AUDIO_RECORDER_CONTROL_INPUT_TERMINAL_ID, // fixed: use input terminal
    0x00U,
    USB_AUDIO_FORMAT_TYPE_I,
    0x01U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,

    /* FORMAT_TYPE_I (Mic) */
    0x06U,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_FORMAT_TYPE,
    USB_AUDIO_FORMAT_TYPE_I,
    0x01U,
#if defined(AUDIO_DATA_SOURCE_DMIC) && (AUDIO_DATA_SOURCE_DMIC > 0U)
    0x02U,
    0x10U,
#elif defined(AUDIO_DATA_SOURCE_PDM) && (AUDIO_DATA_SOURCE_PDM > 0U)
    0x03U,
    0x18U,
#else
    0x01U,
    0x08U,
#endif

    /* Standard ISO IN Endpoint (Mic) */
    USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH,
    USB_DESCRIPTOR_TYPE_ENDPOINT,
    (USB_AUDIO_STREAM_ENDPOINT | (USB_IN << 7)),
    USB_ENDPOINT_ISOCHRONOUS,
    USB_SHORT_GET_LOW(FS_ISO_IN_ENDP_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_ISO_IN_ENDP_PACKET_SIZE),
    ISO_IN_ENDP_INTERVAL,

    /* Class-Specific ISO IN Endpoint Descriptor (Mic) */
    USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH,
    USB_AUDIO_STREAM_ENDPOINT_DESCRIPTOR,
    USB_AUDIO_EP_GENERAL_DESCRIPTOR_SUBTYPE,
    0x00U,
    0x00U,
    0x00U,
    0x00U,

    /* --- Speaker Streaming (AS) Alt-0 --- */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_STREAM_INTERFACE_INDEX + 1,
    USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_0,
    0x00U,
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOSTREAM,
    USB_AUDIO_PROTOCOL,
    0x00U,

    /* --- Speaker Streaming (AS) Alt-1 --- */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_STREAM_INTERFACE_INDEX + 1,
    USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_1,
    0x01U,
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOSTREAM,
    USB_AUDIO_PROTOCOL,
    0x00U,

    /* Class-Specific AS_GENERAL (Speaker) */
    0x10U,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_AS_GENERAL,
    USB_AUDIO_RECORDER_CONTROL_OUTPUT_TERMINAL_ID,
    0x00U,
    USB_AUDIO_FORMAT_TYPE_I,
    0x01U,
    0x00U,
    0x00U,
    0x00U,
    0x00U,

    /* FORMAT_TYPE_I (Speaker) */
    0x06U,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_FORMAT_TYPE,
    USB_AUDIO_FORMAT_TYPE_I,
    0x01U,
    0x02U,
    0x10U,

    /* Standard ISO OUT Endpoint (Speaker) */
    USB_AUDIO_STANDARD_AS_ISO_DATA_ENDPOINT_LENGTH,
    USB_DESCRIPTOR_TYPE_ENDPOINT,
    (USB_AUDIO_STREAM_ENDPOINT | (USB_OUT << 7)),
    USB_ENDPOINT_ISOCHRONOUS,
    USB_SHORT_GET_LOW(FS_ISO_OUT_ENDP_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_ISO_OUT_ENDP_PACKET_SIZE),
    ISO_OUT_ENDP_INTERVAL,

    /* Class-Specific ISO OUT Endpoint Descriptor (Speaker) */
    USB_AUDIO_CLASS_SPECIFIC_ENDPOINT_LENGTH,
    USB_AUDIO_STREAM_ENDPOINT_DESCRIPTOR,
    USB_AUDIO_EP_GENERAL_DESCRIPTOR_SUBTYPE,
    0x00U,
    0x00U,
    0x00U,
    0x00U,

#else /* fall back to Audio Class 1.0 */

    /* ===== USB Audio 1.0 configuration ===== */

    /* Configuration Descriptor */
    USB_DESCRIPTOR_LENGTH_CONFIGURE,
    USB_DESCRIPTOR_TYPE_CONFIGURE,
    USB_SHORT_GET_LOW(
        USB_DESCRIPTOR_LENGTH_CONFIGURE + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH + USB_AUDIO_INPUT_TERMINAL_ONLY_DESC_SIZE + USB_AUDIO_FEATURE_UNIT_ONLY_DESC_SIZE + USB_AUDIO_OUTPUT_TERMINAL_ONLY_DESC_SIZE + USB_DESCRIPTOR_LENGTH_AC_INTERRUPT_ENDPOINT + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_AUDIO_STREAMING_IFACE_DESC_SIZE + USB_AUDIO_STREAMING_TYPE_I_DESC_SIZE + USB_ENDPOINT_AUDIO_DESCRIPTOR_LENGTH + USB_AUDIO_STREAMING_ENDP_DESC_SIZE),
    USB_SHORT_GET_HIGH(
        USB_DESCRIPTOR_LENGTH_CONFIGURE + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH + USB_AUDIO_INPUT_TERMINAL_ONLY_DESC_SIZE + USB_AUDIO_FEATURE_UNIT_ONLY_DESC_SIZE + USB_AUDIO_OUTPUT_TERMINAL_ONLY_DESC_SIZE + USB_DESCRIPTOR_LENGTH_AC_INTERRUPT_ENDPOINT + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_INTERFACE + USB_AUDIO_STREAMING_IFACE_DESC_SIZE + USB_AUDIO_STREAMING_TYPE_I_DESC_SIZE + USB_ENDPOINT_AUDIO_DESCRIPTOR_LENGTH + USB_AUDIO_STREAMING_ENDP_DESC_SIZE),
    USB_AUDIO_GENERATOR_INTERFACE_COUNT, /* bNumInterfaces */
    USB_AUDIO_GENERATOR_CONFIGURE_INDEX, /* bConfigurationValue */
    0x00U,                               /* iConfiguration */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK)
#if defined(USB_DEVICE_CONFIG_SELF_POWER) && (USB_DEVICE_CONFIG_SELF_POWER > 0U)
        | (1U << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT)
#endif
#if defined(USB_DEVICE_CONFIG_REMOTE_WAKEUP) && (USB_DEVICE_CONFIG_REMOTE_WAKEUP > 0U)
        | (1U << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT)
#endif
        ,                 /* bmAttributes */
    USB_DEVICE_MAX_POWER, /* bMaxPower */

    /* Standard AC Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_CONTROL_INTERFACE_INDEX,
    USB_AUDIO_GENERATOR_CONTROL_INTERFACE_ALTERNATE_0,
    0x01U, /* bNumEndpoints = 1 (interrupt EP) */
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOCONTROL,
    USB_AUDIO_PROTOCOL,
    0x00U, /* iInterface */

    /* CS Interface Header (AC) */
    USB_AUDIO_CONTROL_INTERFACE_HEADER_LENGTH,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_HEADER,
    0x00U, /* bcdADC = 1.00 */
    0x01U,
    0x27,  /* wTotalLength L */
    0x00U, /* wTotalLength H */
    0x01U, /* bInCollection = 1 streaming IF */
    0x01U, /* baInterfaceNr(1) */

    /* Input Terminal */
    USB_AUDIO_INPUT_TERMINAL_ONLY_DESC_SIZE,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_INPUT_TERMINAL,
    USB_AUDIO_RECORDER_CONTROL_INPUT_TERMINAL_ID,
    0x01U, /* TerminalType = Microphone */
    0x02U,
    0x00U, /* bAssocTerminal */
    0x01U, /* bNrChannels */
    0x00U,
    0x00U, /* wChannelConfig */
    0x00U, /* iChannelNames */
    0x00U, /* iTerminal */

    /* Feature Unit */
    USB_AUDIO_FEATURE_UNIT_ONLY_DESC_SIZE,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_FEATURE_UNIT,
    USB_AUDIO_RECORDER_CONTROL_FEATURE_UNIT_ID,
    USB_AUDIO_RECORDER_CONTROL_INPUT_TERMINAL_ID,
    0x01U, /* bmaControls(0) */
    0x03U, /* Mute & Volume */
    0x00U, /* iFeature */

    /* Output Terminal */
    USB_AUDIO_OUTPUT_TERMINAL_ONLY_DESC_SIZE,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_OUTPUT_TERMINAL,
    USB_AUDIO_RECORDER_CONTROL_OUTPUT_TERMINAL_ID,
    0x01U, /* TerminalType = USB Streaming */
    0x01U,
    0x00U, /* bAssocTerminal */
    USB_AUDIO_RECORDER_CONTROL_FEATURE_UNIT_ID,
    0x00U, /* iTerminal */

    /* Interrupt EP Descriptor (AC) */
    USB_DESCRIPTOR_LENGTH_AC_INTERRUPT_ENDPOINT,
    USB_DESCRIPTOR_TYPE_ENDPOINT,
    USB_AUDIO_CONTROL_ENDPOINT | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    USB_ENDPOINT_INTERRUPT,
    USB_SHORT_GET_LOW(FS_INTERRUPT_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_INTERRUPT_IN_PACKET_SIZE),
    FS_INTERRUPT_IN_INTERVAL,
    0x00U,

    /* --- AudioStreaming Interface, Alt 0 --- */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_STREAM_INTERFACE_INDEX,
    USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_0,
    0x00U, /* bNumEndpoints = 0 */
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOSTREAM,
    USB_AUDIO_PROTOCOL,
    0x00U, /* iInterface */

    /* --- AudioStreaming Interface, Alt 1 --- */
    USB_DESCRIPTOR_LENGTH_INTERFACE,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_AUDIO_STREAM_INTERFACE_INDEX,
    USB_AUDIO_GENERATOR_STREAM_INTERFACE_ALTERNATE_1,
    0x01U, /* bNumEndpoints = 1 */
    USB_AUDIO_CLASS,
    USB_SUBCLASS_AUDIOSTREAM,
    USB_AUDIO_PROTOCOL,
    0x00U, /* iInterface */

    /* AS General Descriptor */
    USB_AUDIO_STREAMING_IFACE_DESC_SIZE,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_AS_GENERAL,
    USB_AUDIO_RECORDER_CONTROL_OUTPUT_TERMINAL_ID,
    0x00U, /* bDelay */
    0x01U, /* wFormatTag = PCM */
    0x00U,

    /* Type I Format */
    USB_AUDIO_STREAMING_TYPE_I_DESC_SIZE,
    USB_DESCRIPTOR_TYPE_AUDIO_CS_INTERFACE,
    USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_FORMAT_TYPE,
    USB_AUDIO_FORMAT_TYPE_I,
    0x01U,            /* bNrChannels */
    0x01U,            /* bSubFrameSize */
    0x08U,            /* bBitResolution */
    0x01U,            /* bSamFreqType */
    0x80, 0x3E, 0x00, /* 16 kHz */

    /* Standard Isochronous Audio Data Endpoint (AS) */
    USB_ENDPOINT_AUDIO_DESCRIPTOR_LENGTH,
    USB_DESCRIPTOR_TYPE_ENDPOINT,
    USB_AUDIO_STREAM_ENDPOINT | (USB_IN << 7U),
    USB_ENDPOINT_ISOCHRONOUS,
    USB_SHORT_GET_LOW(FS_ISO_IN_ENDP_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_ISO_IN_ENDP_PACKET_SIZE),
    ISO_IN_ENDP_INTERVAL,

    /* Audio Data Endpoint descriptor */
    USB_AUDIO_STREAMING_ENDP_DESC_SIZE,
    USB_AUDIO_STREAM_ENDPOINT_DESCRIPTOR,
    USB_AUDIO_EP_GENERAL_DESCRIPTOR_SUBTYPE,
    0x00U,        /* bmAttributes */
    0x00U,        /* bLockDelayUnits */
    0x00U, 0x00U, /* wLockDelay */

#endif /* USB_DEVICE_CONFIG_AUDIO_CLASS_2_0 */
};

/* Define string descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString0[] = {
    2U + 2U,
    USB_DESCRIPTOR_TYPE_STRING,
    0x09U,
    0x04U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString1[] = {
    2U + 2U * 18U,
    USB_DESCRIPTOR_TYPE_STRING,
    'N',
    0x00U,
    'X',
    0x00U,
    'P',
    0x00U,
    ' ',
    0x00U,
    'S',
    0x00U,
    'E',
    0x00U,
    'M',
    0x00U,
    'I',
    0x00U,
    'C',
    0x00U,
    'O',
    0x00U,
    'N',
    0x00U,
    'D',
    0x00U,
    'U',
    0x00U,
    'C',
    0x00U,
    'T',
    0x00U,
    'O',
    0x00U,
    'R',
    0x00U,
    'S',
    0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString2[] = {
    2U + 2U * 14U,
    USB_DESCRIPTOR_TYPE_STRING,
    'U',
    0x00U,
    'S',
    0x00U,
    'B',
    0x00U,
    ' ',
    0x00U,
    'A',
    0x00U,
    'U',
    0x00U,
    'D',
    0x00U,
    'I',
    0x00U,
    'O',
    0x00U,
    ' ',
    0x00U,
    'D',
    0x00U,
    'E',
    0x00U,
    'M',
    0x00U,
    'O',
    0x00U,
};

uint32_t g_UsbDeviceStringDescriptorLength[USB_DEVICE_STRING_COUNT] = {
    sizeof(g_UsbDeviceString0),
    sizeof(g_UsbDeviceString1),
    sizeof(g_UsbDeviceString2),
};

uint8_t *g_UsbDeviceStringDescriptorArray[USB_DEVICE_STRING_COUNT] = {
    g_UsbDeviceString0,
    g_UsbDeviceString1,
    g_UsbDeviceString2,
};

usb_language_t g_UsbDeviceLanguage[USB_DEVICE_LANGUAGE_COUNT] = {{
    g_UsbDeviceStringDescriptorArray,
    g_UsbDeviceStringDescriptorLength,
    (uint16_t)0x0409U,
}};

usb_language_list_t g_UsbDeviceLanguageList = {
    g_UsbDeviceString0,
    sizeof(g_UsbDeviceString0),
    g_UsbDeviceLanguage,
    USB_DEVICE_LANGUAGE_COUNT,
};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Get the descriptor.
 *
 * The function is used to get the descriptor, including the device descriptor, configuration descriptor, and string
 * descriptor, etc.
 *
 * @param handle              The device handle.
 * @param setup               The setup packet buffer address.
 * @param length              It is an OUT parameter, return the data length need to be sent to host.
 * @param buffer              It is an OUT parameter, return the data buffer address.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
/*! @brief Get a descriptor (device, configuration or string). */
usb_status_t USB_DeviceGetDescriptor(usb_device_handle handle,
                                     usb_setup_struct_t *setup,
                                     uint32_t *length,
                                     uint8_t **buffer)
{
    uint8_t descriptorType = (uint8_t)((setup->wValue & 0xFF00U) >> 8U);
    uint8_t descriptorIndex = (uint8_t)((setup->wValue & 0x00FFU));
    usb_status_t ret = kStatus_USB_Success;

    if (USB_REQUEST_STANDARD_GET_DESCRIPTOR != setup->bRequest)
    {
        return kStatus_USB_InvalidRequest;
    }

    switch (descriptorType)
    {
    case USB_DESCRIPTOR_TYPE_STRING:
        if (descriptorIndex == 0U)
        {
            *buffer = (uint8_t *)g_UsbDeviceLanguageList.languageString;
            *length = g_UsbDeviceLanguageList.stringLength;
        }
        else
        {
            uint8_t languageId = 0U;
            uint8_t languageIndex = USB_DEVICE_STRING_COUNT;
            for (; languageId < USB_DEVICE_LANGUAGE_COUNT; languageId++)
            {
                if (setup->wIndex == g_UsbDeviceLanguageList.languageList[languageId].languageId)
                {
                    if (descriptorIndex < USB_DEVICE_STRING_COUNT)
                    {
                        languageIndex = descriptorIndex;
                    }
                    break;
                }
            }
            if (languageIndex == USB_DEVICE_STRING_COUNT)
            {
                return kStatus_USB_InvalidRequest;
            }
            *buffer = (uint8_t *)g_UsbDeviceLanguageList.languageList[languageId].string[languageIndex];
            *length = g_UsbDeviceLanguageList.languageList[languageId].length[languageIndex];
        }
        break;

    case USB_DESCRIPTOR_TYPE_DEVICE:
        *buffer = g_UsbDeviceDescriptor;
        *length = USB_DESCRIPTOR_LENGTH_DEVICE;
        break;

    case USB_DESCRIPTOR_TYPE_CONFIGURE:
        *buffer = g_UsbDeviceConfigurationDescriptor;
        *length = USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL;
        break;

    default:
        ret = kStatus_USB_InvalidRequest;
        break;
    }

    return ret;
}

/*! @brief Set the current configuration. */
usb_status_t USB_DeviceSetConfigure(usb_device_handle handle, uint8_t configure)
{
    if (configure == 0U)
    {
        return kStatus_USB_Error;
    }
    g_UsbDeviceCurrentConfigure = configure;
    return USB_DeviceCallback(handle, kUSB_DeviceEventSetConfiguration, &configure);
}

/*! @brief Get the current configuration. */
usb_status_t USB_DeviceGetConfigure(usb_device_handle handle, uint8_t *configure)
{
    *configure = g_UsbDeviceCurrentConfigure;
    return kStatus_USB_Success;
}

/*! @brief Set an alternate interface setting. */
usb_status_t USB_DeviceSetInterface(usb_device_handle handle, uint8_t interface, uint8_t alternateSetting)
{
    if (interface < USB_AUDIO_GENERATOR_INTERFACE_COUNT)
    {
        g_UsbDeviceInterface[interface] = alternateSetting;
        return USB_DeviceCallback(handle, kUSB_DeviceEventSetInterface, &interface);
    }
    return kStatus_USB_InvalidRequest;
}

/*! @brief Get the current alternate interface setting. */
usb_status_t USB_DeviceGetInterface(usb_device_handle handle, uint8_t interface, uint8_t *alternateSetting)
{
    if (interface < USB_AUDIO_GENERATOR_INTERFACE_COUNT)
    {
        *alternateSetting = g_UsbDeviceInterface[interface];
        return kStatus_USB_Success;
    }
    return kStatus_USB_InvalidRequest;
}

/*! @brief Patch all endpoint descriptors to match FS/HS after enumeration. */
usb_status_t USB_DeviceSetSpeed(uint8_t speed)
{
    usb_descriptor_union_t *head = (usb_descriptor_union_t *)&g_UsbDeviceConfigurationDescriptor[0];
    usb_descriptor_union_t *tail = (usb_descriptor_union_t *)&g_UsbDeviceConfigurationDescriptor
        [USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL - 1U];

    while (head < tail)
    {
        if (head->common.bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
        {
            bool isAudioStreamEp = ((head->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK) ==
                                    USB_AUDIO_STREAM_ENDPOINT);
            if (speed == USB_SPEED_HIGH)
            {
                head->endpoint.bInterval = isAudioStreamEp ? HS_ISO_IN_ENDP_INTERVAL
                                                           : HS_INTERRUPT_IN_INTERVAL;
                USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(
                    isAudioStreamEp ? HS_ISO_IN_ENDP_PACKET_SIZE : HS_INTERRUPT_IN_PACKET_SIZE,
                    head->endpoint.wMaxPacketSize);
            }
            else
            {
                head->endpoint.bInterval = isAudioStreamEp ? FS_ISO_IN_ENDP_INTERVAL
                                                           : FS_INTERRUPT_IN_INTERVAL;
                USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(
                    isAudioStreamEp ? FS_ISO_IN_ENDP_PACKET_SIZE : FS_INTERRUPT_IN_PACKET_SIZE,
                    head->endpoint.wMaxPacketSize);
            }
        }
        head = (usb_descriptor_union_t *)((uint8_t *)head + head->common.bLength);
    }

    return kStatus_USB_Success;
}
