// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Common 3D functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_3D_H__
#define NF_3D_H__

#include <nds.h>

/// @file   nf_3d.h
/// @brief  Functions to setup 3D video options.

/// @defgroup nf_3d 3D video setup
///
/// Functions to setup 2D the video modes of the NDS and enable 3D.
///
/// @{

/// Init 3D mode for the selected screen.
///
/// Modes:
/// 0 - Tiled BGs (256 colors).
/// 2 - Affine BGs of 8 bits in layers 2 & 3
/// 5 - Bitmap BGs at 8 or 16 bits.
///
/// The 3D output replaces layer 0.
///
/// If you set screen 1 for 3D, the screen numbers for 2D will be inverted (the
/// top screen will be 1 and the bottom screen will be 0).
///
/// You must use this function before using 3D sprites.
///
/// Example:
/// ```
/// // Setup video mode 0 (tiled backgrounds) with 3D
/// NF_Set3D(1, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param mode Mode (0, 2, 5).
void NF_Set3D(int screen, u32 mode);

/// Initialitzes and configures OpenGL for 3D sprites.
///
/// NF_Init3dSpriteSys() automaticaly calls it, so the user doesn't need to call
/// it manually.
void NF_InitOpenGL(void);

// Internal use. It takes a texture size in pixels and returns the internal DS
// hardware representation of that size.
u32 NF_GetTextureSize(u32 textel);

/// @}

#endif // NF_3D_H__

#ifdef __cplusplus
}
#endif
