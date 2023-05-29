// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Common 2D functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_2D_H__
#define NF_2D_H__

/// @file   nf_2d.h
/// @brief  Functions to setup 2D video options.

/// @defgroup nf_2d 2D video setup
///
/// Functions to setup the 2D video modes of the NDS, as well as to activate or
/// deactivate background layers or scroll them.
///
/// @{

#include <nds.h>

#include "nf_sprite256.h"

/// Init 2D mode for the selected screen.
///
/// Modes:
/// 0 - Tiled BGs (256 colors).
/// 2 - Affine BGs of 8 bits in layers 2 & 3
/// 5 - Bitmap BGs at 8 or 16 bits.
///
/// Example:
/// ```
/// // Init 2D mode for tiled BGs and sprites on screen 1 (bottom)
/// NF_Set2D(1, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param mode Mode (0, 2, 5).
void NF_Set2D(int screen, u32 mode);

/// Makes the BG of the selected layer and screen visible.
///
/// Use this command to make visible a BG previously hidden with NF_HideBg().
///
/// Example:
/// ```
/// // Makes the BG of layer 2 in screen 0 (top) visible
/// NF_ShowBg(0, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_ShowBg(int screen, u32 layer);

/// Makes the BG of the selected layer and screen invisible.
///
/// Example:
/// ```
/// // Makes the BG of layer 2 in screen 0 (top) invisible
/// NF_HideBg(0, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_HideBg(int screen, u32 layer);

/// Moves the BG of the selected layer and screen to the specified coordinates.
///
/// If the map is taller or wider than 512, it must be kept in RAM all the time.
///
/// Example:
/// ```
/// // Move the BG of layer 1 and screen 0 to the coordinates (128, 96)
/// NF_ScrollBg(0, 1, 128, 96);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param x X coordinate.
/// @param y Y coordinate.
void NF_ScrollBg(int screen, u32 layer, s32 x, s32 y);

/// Moves the selected sprite of a screen to the specified coordinates.
///
/// Example:
/// ```
/// // Moves sprite 35 of screen 0 to the coordinates (100, 50).
/// NF_MoveSprite(0, 35, 100, 50);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param x X coordinate.
/// @param y Y coordinate.
static inline void NF_MoveSprite(int screen, u32 id, s32 x, s32 y)
{
    NF_SPRITEOAM[screen][id].x = x;
    NF_SPRITEOAM[screen][id].y = y;
}

/// Selects the layer where a sprite will be drawn.
///
/// Layer 0 is on top of the others, layer 3 is at the bottom.
///
/// Example:
/// ```
/// // Sprite 35 of screen 1 will be drawn over layer 2.
/// NF_SpriteLayer(1, 35, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param layer Layer (0 - 3).
static inline void NF_SpriteLayer(int screen, u32 id, u32 layer)
{
    NF_SPRITEOAM[screen][id].layer = layer;
}

/// Shows or hides a sprite.
///
/// If you hide a sprite, it becomes invisible, without deleting it.
///
/// Example:
/// ```
/// // Hides sprite 35 of screen 0
/// NF_ShowSprite(0, 35, false);
/// // Makes sprite 45 of screen 1 visible
/// NF_ShowSprite(1, 45, true);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param show Set to true to show the sprite, false otherwise.
static inline void NF_ShowSprite(int screen, u32 id, bool show)
{
    NF_SPRITEOAM[screen][id].hide = !show;
}

/// Sets the horizontal flip state of a sprite.
///
/// Example:
/// ```
/// // Flips sprite 35 of screen 0 horizontally
/// NF_HflipSprite(0, 35, true);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param hflip Set to true to flip the sprite, false otherwise.
static inline void NF_HflipSprite(int screen, u32 id, bool hflip)
{
    NF_SPRITEOAM[screen][id].hflip = hflip;
}

/// Gets the horizontal flip state of a sprite.
///
/// Example:
/// ```
/// // Stores true in "state" if sprite 35 of screen 0 is flipped
/// bool hflip = NF_GetSpriteHflip(0, 35);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @return hflip Returns true if the sprite is flipped, false otherwise.
static inline bool NF_GetSpriteHflip(int screen, u32 id)
{
    return NF_SPRITEOAM[screen][id].hflip;
}

/// Sets the vertical flip state of a sprite.
///
/// Example:
/// ```
/// // Flips sprite 35 of screen 0 vertically
/// NF_VflipSprite(0, 35, true);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param vflip Set to true to flip the sprite, false otherwise.
static inline void NF_VflipSprite(int screen, u32 id, bool vflip)
{
    NF_SPRITEOAM[screen][id].vflip = vflip;
}

/// Gets the vertical flip state of a sprite.
///
/// Example:
/// ```
/// // Stores true in "state" if sprite 35 of screen 0 is flipped
/// bool hflip = NF_GetSpriteVflip(0, 35);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @return vflip Returns true if the sprite is flipped, false otherwise.
static inline bool NF_GetSpriteVflip(int screen, u32 id)
{
    return NF_SPRITEOAM[screen][id].vflip;
}

/// Selects which frame of the animation of a sprite is shown.
///
/// Example:
/// ```
/// // Sprite 20 of screen 0 shows frame 5.
/// NF_SpriteFrame(0, 20, 5);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param frame The frame to show.
void NF_SpriteFrame(int screen, u32 id, u32 frame);

/// Makes a sprite available to be rotated and scaled.
///
/// You must specify which rotset of the 32 availables will be used by the
/// sprite. Multiple sprites can share the same rotset. If "doublesize" is
/// disabled, the max size for sprites is 32x32. Otherwise the sprite will be
/// clipped.
///
/// If you enable RotScale for a sprite the flip states will be ignored.
///
/// Example:
/// ```
/// // Enables the rotation and scalation for sprite 111 in screen 1, using the
/// // RotSet 12, with “doublesize” disabled.
/// NF_EnableSpriteRotScale(1, 111, 12, false);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param sprite Sprite ID (0 - 127).
/// @param id RotSet ID (0 - 31).
/// @param doublesize Set to true to enable double size mode.
void NF_EnableSpriteRotScale(int screen, u32 sprite, u32 id, bool doublesize);

/// Disables the rotation and scalation of sprite.
///
/// Example:
/// ```
/// // Disable rotation and scalation of sprite 46 of screen 0.
/// NF_DisableSpriteRotScale(0, 46);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param sprite Sprite ID (0 - 127).
void NF_DisableSpriteRotScale(int screen, u32 sprite);

/// Setup the rotation and scalation values of a RotSet.
///
/// All sprites assigned to this RotSet will rotate and scale using those
/// values. Rotation angles are in 512 base. This means the rotation will go
/// from -512 to 512 (-360 to 360 degrees). Scale values go from 0 to 512. A
/// 100% scale is 256.
///
/// Example:
/// ```
/// // Rotate all sprites with RotSet 16 by 90 degrees to the right, with 100%
/// // scale in both directions, on screen 0.
/// NF_SpriteRotScale(0, 16, 128, 256, 256);
/// // Rotate all sprites with RotSet 10 by 180 degrees to the left, scaling the
/// // X size by 200% and the Y size by 100%, on screen 1.
/// NF_SpriteRotScale(1, 10, -256, 512, 256);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param angle Angle (-512 to 512).
/// @param sx X scale (0 to 512), 100% = 256.
/// @param sy Y scale (0 to 512), 100% = 256.
void NF_SpriteRotScale(int screen, u8 id, s32 angle, u32 sx, u32 sy);

/// @}

#endif // NF_2D_H__

#ifdef __cplusplus
}
#endif
