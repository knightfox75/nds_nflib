// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Mixed (tiled and 8-bit bitmap) background functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_MIXEDBG_H__
#define NF_MIXEDBG_H__

#include <nds.h>

/// @file   nf_mixedbg.h
/// @brief  Helpers to use bitmap and tiled backgrounds at the same time.

/// @defgroup nf_mixedbg Helpers to use bitmap and tiled backgrounds.
///
/// Helpers to use bitmap and tiled backgrounds at the same time.
///
/// @{

/// Initialize mixed background mode (tiled background and 8-bit bitmap).
///
/// Layers 0 to 2: Tiled, 64 KB (48 KB for tiles, 16 KB for maps).
/// Layer 3: 8-bit bitmap, 64 KB.
///
/// After using this function you can use functions of both background modes.
///
/// @param screen Screen (0 - 1).
void NF_InitMixedBgSys(int screen);

/// @}

#endif // NF_MIXEDBG_H__

#ifdef __cplusplus
}
#endif
