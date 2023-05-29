// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Media file loading functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_MEDIA_H__
#define NF_MEDIA_H__

#include <nds.h>

/// @file   nf_media.h
/// @brief  Functions to load files of common media formats.

/// @defgroup nf_media Functions to load files of common media formats.
///
/// This module contains a function to load BMP files.
///
/// @{

/// Load a BMP image into a 16-bit background slot.
///
/// It supports 8, 16 and 24 bits BMP images. To load and show the image, you
/// must initialize 16 bits mode, the backbuffers and to call
/// NF_Draw16bitsImage() to send the image from the RAM slot to the backbuffer.
///
/// All pixels drawn out of bounds are ignored.
///
/// Example:
/// ```
/// // Load "lostend.bmp" to 16-bit slot 0
/// NF_LoadBMP("bmp/lostend", 0);
/// ```
///
/// @param file File path.
/// @param slot Slot number (0 - 15).
void NF_LoadBMP(const char *file, u32 slot);

/// @}

#endif // NF_MEDIA_H__

#ifdef __cplusplus
}
#endif
