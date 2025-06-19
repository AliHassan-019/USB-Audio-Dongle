/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_DEVICE_CONFIG_H_
#define _USB_DEVICE_CONFIG_H_

#define USB_DEVICE_CONFIG_KHCI                         1
#define USB_DEVICE_CONFIG_KHCI_DMA_ALIGN_BUFFER_LENGTH 64
#define USB_DEVICE_CONFIG_EHCI                         0
#define USB_DEVICE_CONFIG_EHCI_MAX_DTD                 16
#define USB_DEVICE_CONFIG_LPCIP3511FS                  0
#define USB_DEVICE_CONFIG_LPCIP3511HS                  0
#define USB_DEVICE_CONFIG_NUM                          \
    (USB_DEVICE_CONFIG_KHCI +                          \
     USB_DEVICE_CONFIG_EHCI +                          \
     USB_DEVICE_CONFIG_LPCIP3511FS +                   \
     USB_DEVICE_CONFIG_LPCIP3511HS)

#define USB_DEVICE_CONFIG_AUDIO                        1

/* Use USB Audio Class 1.0 (set CLASS_2_0 to 0) */
#define USB_DEVICE_CONFIG_AUDIO_CLASS_2_0              0

#define USB_DEVICE_CONFIG_CDC_ACM                      0
#define USB_DEVICE_CONFIG_CDC_ECM                      0
#define USB_DEVICE_CONFIG_CDC_RNDIS                    0
#define USB_DEVICE_CONFIG_HID                          0
#define USB_DEVICE_CONFIG_PRINTER                      0
#define USB_DEVICE_CONFIG_MSC                          0
#define USB_DEVICE_CONFIG_CCID                         0
#define USB_DEVICE_CONFIG_VIDEO                        0
#define USB_DEVICE_CONFIG_PHDC                         0
#define USB_DEVICE_CONFIG_DFU                          0
#define USB_DEVICE_CONFIG_MTP                          0

/* Self-powered device */
#define USB_DEVICE_CONFIG_SELF_POWER                   1

/* Control EP + interrupt EP + ISO IN + ISO OUT = 4 endpoints */
#define USB_DEVICE_CONFIG_ENDPOINTS                    4

#define USB_DEVICE_CONFIG_MAX_MESSAGES                  8

#endif /* _USB_DEVICE_CONFIG_H_ */
