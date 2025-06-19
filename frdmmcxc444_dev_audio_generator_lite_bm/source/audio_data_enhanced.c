/*
 * Enhanced Audio Data Management for USB-C Audio Dongle
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016,2019 NXP
 * Enhanced 2024 for USB-C Audio Dongle Project
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_descriptor.h"
#include "fsl_debug_console.h"
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Enhanced audio buffer management */
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
#define AUDIO_ENDPOINT_MAX_PACKET_SIZE \
    (FS_ISO_IN_ENDP_PACKET_SIZE > HS_ISO_IN_ENDP_PACKET_SIZE ? FS_ISO_IN_ENDP_PACKET_SIZE : HS_ISO_IN_ENDP_PACKET_SIZE)
#endif

#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0U)
#define AUDIO_ENDPOINT_MAX_PACKET_SIZE (FS_ISO_IN_ENDP_PACKET_SIZE)
#endif

#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
#define AUDIO_ENDPOINT_MAX_PACKET_SIZE (FS_ISO_IN_ENDP_PACKET_SIZE)
#endif

/* Enhanced buffer size for better audio quality */
#define AUDIO_BUFFER_SIZE (AUDIO_ENDPOINT_MAX_PACKET_SIZE * 4)
#define AUDIO_WAVE_SAMPLES 1000

/* Logging macros */
#define AUDIO_DATA_LOG_ENABLE 1

#if AUDIO_DATA_LOG_ENABLE
#define AUDIO_DATA_LOG(fmt, ...) PRINTF("[AUDIO_DATA] " fmt "\r\n", ##__VA_ARGS__)
#define AUDIO_DATA_DEBUG(fmt, ...) PRINTF("[AUDIO_DATA_DEBUG] " fmt "\r\n", ##__VA_ARGS__)
#else
#define AUDIO_DATA_LOG(fmt, ...)
#define AUDIO_DATA_DEBUG(fmt, ...)
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Enhanced audio buffer management */
static uint32_t s_audioBufferIndex = 0;
static uint32_t s_totalSamplesGenerated = 0;
static uint8_t s_audioPattern = 0; /* 0=sine wave, 1=square wave, 2=silence */

/* Enhanced audio wave data - higher quality sine wave */
#if defined(__cpluscplus)
extern const unsigned char wavData[] = {
#else
const unsigned char wavData[] = {
#endif
    /* High-quality 48kHz compatible sine wave data */
    0x80, 0x83, 0x86, 0x89, 0x8C, 0x8F, 0x92, 0x95, 0x98, 0x9B, 0x9E, 0xA1, 0xA4, 0xA7, 0xAA, 0xAD,
    0xB0, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC2, 0xC5, 0xC8, 0xCB, 0xCE, 0xD1, 0xD4, 0xD7, 0xDA, 0xDD,
    0xE0, 0xE3, 0xE6, 0xE9, 0xEC, 0xEF, 0xF2, 0xF5, 0xF8, 0xFB, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFB,
    0xF8, 0xF5, 0xF2, 0xEF, 0xEC, 0xE9, 0xE6, 0xE3, 0xE0, 0xDD, 0xDA, 0xD7, 0xD4, 0xD1, 0xCE, 0xCB,
    0xC8, 0xC5, 0xC2, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3, 0xB0, 0xAD, 0xAA, 0xA7, 0xA4, 0xA1, 0x9E, 0x9B,
    0x98, 0x95, 0x92, 0x8F, 0x8C, 0x89, 0x86, 0x83, 0x80, 0x7D, 0x7A, 0x77, 0x74, 0x71, 0x6E, 0x6B,
    0x68, 0x65, 0x62, 0x5F, 0x5C, 0x59, 0x56, 0x53, 0x50, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3E, 0x3B,
    0x38, 0x35, 0x32, 0x2F, 0x2C, 0x29, 0x26, 0x23, 0x20, 0x1D, 0x1A, 0x17, 0x14, 0x11, 0x0E, 0x0B,
    0x08, 0x05, 0x02, 0x01, 0x00, 0x01, 0x02, 0x05, 0x08, 0x0B, 0x0E, 0x11, 0x14, 0x17, 0x1A, 0x1D,
    0x20, 0x23, 0x26, 0x29, 0x2C, 0x2F, 0x32, 0x35, 0x38, 0x3B, 0x3E, 0x41, 0x44, 0x47, 0x4A, 0x4D,
    0x50, 0x53, 0x56, 0x59, 0x5C, 0x5F, 0x62, 0x65, 0x68, 0x6B, 0x6E, 0x71, 0x74, 0x77, 0x7A, 0x7D
};

/* Enhanced buffer for audio data */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_wavBuff[AUDIO_BUFFER_SIZE];

/* Wave data size */
const uint32_t wavDataSize = sizeof(wavData);

/*******************************************************************************
 * Enhanced Code Implementation
 ******************************************************************************/

/*!
 * @brief Generate square wave pattern
 */
static void USB_AudioGenerateSquareWave(uint8_t *buffer, uint32_t size)
{
    static uint32_t squareCounter = 0;
    const uint32_t halfPeriod = 24; /* Adjust for desired frequency */
    
    for (uint32_t i = 0; i < size; i++)
    {
        buffer[i] = (squareCounter < halfPeriod) ? 0x20 : 0xE0;
        squareCounter++;
        if (squareCounter >= (halfPeriod * 2))
        {
            squareCounter = 0;
        }
    }
}

/*!
 * @brief Generate silence pattern
 */
static void USB_AudioGenerateSilence(uint8_t *buffer, uint32_t size)
{
    memset(buffer, 0x80, size); /* 0x80 is audio zero level for 8-bit unsigned */
}

/*!
 * @brief Enhanced audio buffer management with multiple wave patterns
 */
void USB_AudioRecorderGetBuffer(uint8_t *buffer, uint32_t size)
{
    static uint32_t patternChangeCounter = 0;
    
    if (buffer == NULL || size == 0)
    {
        AUDIO_DATA_LOG("Invalid buffer parameters: buffer=%p, size=%lu", buffer, size);
        return;
    }

    /* Log buffer request periodically */
    if ((s_totalSamplesGenerated % 10000) == 0)
    {
        AUDIO_DATA_DEBUG("Audio buffer requested: size=%lu, total samples=%lu, pattern=%d", 
                        size, s_totalSamplesGenerated, s_audioPattern);
    }

    /* Change audio pattern every 30 seconds (assuming ~1000 calls per second) */
    patternChangeCounter++;
    if (patternChangeCounter >= 30000)
    {
        patternChangeCounter = 0;
        s_audioPattern = (s_audioPattern + 1) % 3;
        
        switch (s_audioPattern)
        {
            case 0:
                AUDIO_DATA_LOG("Switching to sine wave pattern");
                break;
            case 1:
                AUDIO_DATA_LOG("Switching to square wave pattern");
                break;
            case 2:
                AUDIO_DATA_LOG("Switching to silence pattern");
                break;
        }
    }

    /* Generate audio data based on current pattern */
    switch (s_audioPattern)
    {
        case 0: /* Sine wave */
            for (uint32_t i = 0; i < size; i++)
            {
                buffer[i] = wavData[s_audioBufferIndex % wavDataSize];
                s_audioBufferIndex++;
            }
            break;
            
        case 1: /* Square wave */
            USB_AudioGenerateSquareWave(buffer, size);
            break;
            
        case 2: /* Silence */
            USB_AudioGenerateSilence(buffer, size);
            break;
            
        default:
            /* Fallback to sine wave */
            s_audioPattern = 0;
            for (uint32_t i = 0; i < size; i++)
            {
                buffer[i] = wavData[s_audioBufferIndex % wavDataSize];
                s_audioBufferIndex++;
            }
            break;
    }

    /* Update statistics */
    s_totalSamplesGenerated += size;

    /* Prevent index overflow */
    if (s_audioBufferIndex >= (wavDataSize * 1000))
    {
        s_audioBufferIndex = 0;
        AUDIO_DATA_DEBUG("Audio buffer index reset to prevent overflow");
    }

    /* Copy to the global buffer for USB transmission */
    if (size <= AUDIO_BUFFER_SIZE)
    {
        memcpy(s_wavBuff, buffer, size);
    }
    else
    {
        AUDIO_DATA_LOG("WARNING: Requested size (%lu) exceeds buffer size (%d), truncating", 
                      size, AUDIO_BUFFER_SIZE);
        memcpy(s_wavBuff, buffer, AUDIO_BUFFER_SIZE);
    }
}

/*!
 * @brief Set audio pattern manually
 */
void USB_AudioSetPattern(uint8_t pattern)
{
    if (pattern < 3)
    {
        s_audioPattern = pattern;
        AUDIO_DATA_LOG("Audio pattern manually set to %d", pattern);
    }
    else
    {
        AUDIO_DATA_LOG("Invalid audio pattern %d, keeping current pattern %d", pattern, s_audioPattern);
    }
}

/*!
 * @brief Get current audio statistics
 */
void USB_AudioGetStatistics(uint32_t *totalSamples, uint32_t *bufferIndex, uint8_t *currentPattern)
{
    if (totalSamples != NULL)
    {
        *totalSamples = s_totalSamplesGenerated;
    }
    
    if (bufferIndex != NULL)
    {
        *bufferIndex = s_audioBufferIndex;
    }
    
    if (currentPattern != NULL)
    {
        *currentPattern = s_audioPattern;
    }
}

/*!
 * @brief Reset audio statistics and buffers
 */
void USB_AudioResetStatistics(void)
{
    s_audioBufferIndex = 0;
    s_totalSamplesGenerated = 0;
    s_audioPattern = 0;
    memset(s_wavBuff, 0x80, AUDIO_BUFFER_SIZE);
    AUDIO_DATA_LOG("Audio statistics and buffers reset");
} 