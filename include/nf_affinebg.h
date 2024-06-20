// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Affine backgrounds functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_AFFINEBG_H__
#define NF_AFFINEBG_H__

#include <nds.h>

#include "nf_tiledbg.h"

/// @file   nf_affinebg.h
/// @brief  Affine background support.

/// @defgroup nf_affinebg Affine background support.
///
/// Functions to load and manage affine tiled backgrounds. Affine backgrounds
/// can be rotated and scaled, unlike regular tiled backgrounds.
///
/// @{

/// Struct that holds information about affine tiled backgrounds.
typedef struct {
    s32 x;          ///< X position
    s32 y;          ///< Y position
    s32 x_center;   ///< X center
    s32 y_center;   ///< Y center
    s32 x_scale;    ///< X zoom (PA)
    s32 y_scale;    ///< Y zoom (PD)
    s32 x_tilt;     ///< X shear (PB)
    s32 y_tilt;     ///< Y shear (PC)
    s32 angle;      ///< Rotation angle
} NF_TYPE_AFFINE_BG;

/// Information of all affine backgrounds.
extern NF_TYPE_AFFINE_BG NF_AFFINE_BG[2][4];

/// Initializes the affine background system of the selected screen.
///
/// Once this function is called, only affine backgrounds can be used, and only
/// layers 2 and 3 are available. Also, affine backgrounds can only hold up to
/// 256 tiles each, and they must share a 256 color palette.
///
/// The 2D hardware needs to be set to video mode 2.
///
/// Example:
/// ```
/// // Initialize affine mode in the top screen
/// NF_InitAffineBgSys(0);
/// ```
///
/// @param screen Screen (0 - 1).
void NF_InitAffineBgSys(int screen);

/// Load an affine background to RAM from the filesystem.
///
/// It is essential to initialize the tiled background buffers before loading
/// any "affine" background. See the documentation of NF_InitTiledBgBuffers().
///
/// Affine backgrounds have to be 256x256 or 512x512 pixels in size and they can
/// have up to 256 unique tiles. All backgrounds in the same screen must share
/// the palette. Use the script grit to convert your backgrounds.
///
/// Example:
/// ```
/// // Load the "waves512" background from the backgrounds folder, name it
/// // "waves" and specify that the background is 512 x 512 pixels.
/// NF_LoadAffineBg("bg/waves512", "waves", 512, 512);
/// ```
///
/// @param file File path without extension.
/// @param name Name used for the BG for other functions.
/// @param width BG width.
/// @param height BG height.
void NF_LoadAffineBg(const char *file, const char *name, u32 width, u32 height);

/// Deletes the specified affine background from RAM.
///
/// Example:
/// ```
/// // Deletes the background "waves" from RAM
/// NF_UnloadAffineBg("waves")
/// ```
///
/// @param name Name of the background.
static inline void NF_UnloadAffineBg(const char *name)
{
    NF_UnloadTiledBg(name);
}

/// Create an affine background in a layer using graphics preloaded in RAM.
///
/// Specify if you want the background infinite (wrap = 1) or not (wrap = 0).
///
/// Example:
/// ```
/// // Create a background on screen 0, layer 3, using the background graphics
/// // "waves", with wrapping enabled.
/// NF_CreateAffineBg (0, 3, "waves", 1);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (2 - 3).
/// @param name Name of the background.
/// @param wrap True to enable wrap around mode.
void NF_CreateAffineBg(int screen, u32 layer, const char *name, u32 wrap);

/// Deletes from VRAM the background of the specified screen and layer.
///
/// Example:
/// ```
/// // Delete the background of the top screen in layer 3
/// NF_DeleteAffineBg(0, 3);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (2 - 3).
void NF_DeleteAffineBg(int screen, u32 layer);

/// Modify the transformation matrix of the specified background.
///
/// You can change the scale of the X and Y axes, as well as their shear.
///
/// Example:
/// ```
/// // Zoom the bottom screen background in layer 3 to the 50% of its size.
/// NF_AffineBgTransform(0, 3, 512, 512, 0, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (2 - 3).
/// @param x_scale Scale X: 0 - 256 (original) - 512 (or more)
/// @param y_scale Scale Y: 0 - 256 (original) - 512 (or more)
/// @param x_tilt Tilt X: 0 - 512 (or more)
/// @param y_tilt Tilt Y: 0 - 512 (or more)
void NF_AffineBgTransform(int screen, u32 layer, s32 x_scale, s32 y_scale,
                          s32 x_tilt, s32 y_tilt);

/// Moves the affine background to the specified position.
///
/// You can also specify the rotation of this background (between -2048 and
/// 2048). Affine backgrounds can't be moved with NF_ScrollBg().
///
/// Example:
/// ```
/// // Move the background of the top screen in layer 3 to coordinates (128, 96)
/// // and rotate it by 45 degrees clockwise.
/// NF_AffineBgMove(0, 3, 128, 96, 256);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (2 - 3).
/// @param x X coordinate.
/// @param y Y coordinate.
/// @param angle Rotation angle (-2048 to 2048).
void NF_AffineBgMove(int screen, u32 layer, s32 x, s32 y, s32 angle);

/// Define the rotation center of the specified affine background.
///
/// Example:
/// ```
/// // Define the center of rotation of the affine background in layer 3 of the
/// // top screen. Set it to coordinates (128, 128).
/// NF_AffineBgCenter(0, 3, 128, 128);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (2 - 3).
/// @param x X coordinate.
/// @param y Y coordinate.
static inline void NF_AffineBgCenter(int screen, u32 layer, s32 x, s32 y)
{
    NF_AFFINE_BG[screen][layer].x_center = x;
    NF_AFFINE_BG[screen][layer].y_center = y;
}

/// @}

#endif // NF_AFFINEBG_H__

#ifdef __cplusplus
}
#endif
