/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_AUDIO_CONFIG_H_
#define _USB_AUDIO_CONFIG_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*— Data source for microphone: 0 = ADC/PCM, 1 = DMIC, 2 = PDM —*/
#define AUDIO_DATA_SOURCE_DMIC                      0

/*— Speaker Control Unit IDs —*/
#define USB_AUDIO_SPEAKER_CONTROL_INPUT_TERMINAL_ID   0x05U
#define USB_AUDIO_SPEAKER_CONTROL_FEATURE_UNIT_ID     0x06U
#define USB_AUDIO_SPEAKER_CONTROL_OUTPUT_TERMINAL_ID  0x07U

/*— Speaker Streaming Interface —*/
#define USB_AUDIO_SPEAKER_STREAM_INTERFACE_INDEX       0x02U
#define USB_AUDIO_SPEAKER_STREAM_INTERFACE_ALTERNATE_0 0U
#define USB_AUDIO_SPEAKER_STREAM_INTERFACE_ALTERNATE_1 1U

/*— ISO OUT Endpoint for Speaker —*/
#define USB_AUDIO_OUT_ENDPOINT                        0x01U
#define FS_ISO_OUT_ENDP_PACKET_SIZE                   (192U)   /* e.g. 48kHz × 2ch × 2 bytes */
#define HS_ISO_OUT_ENDP_PACKET_SIZE                   (FS_ISO_OUT_ENDP_PACKET_SIZE * 8U)
#define ISO_OUT_ENDP_INTERVAL                         1U

#endif /* _USB_AUDIO_CONFIG_H_ */
