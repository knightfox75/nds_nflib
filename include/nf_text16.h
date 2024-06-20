// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 8x16 font text support functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_TEXT16_H__
#define NF_TEXT16_H__

#include <nds.h>

/// @file   nf_text16.h
/// @brief  Text support functions for 8x16 fonts.

/// @defgroup nf_text16 Text support functions for 8x16 fonts.
///
/// Use the follow functions to use text with 8x16 pixel fonts. The normal text
/// fuctions still compatible with this mode.
///
/// @{

/// Number of characters available in a font
#define NF_TEXT_FONT_CHARS_16 127

/// Last valid character in a font
#define NF_TEXT_FONT_LAST_VALID_CHAR_16 113

/// Load 8x16 font and palette files from the filesystem to RAM.
///
/// You must specify the filename without extension and the name you want to
/// give to the font and the size of the text layer you want to create, in
/// pixels.
///
/// If the font includes the characters for rotated text, the values are:
/// - 0: No rotation
/// - 1: Rotate clockwise
/// - 2: Rotate counter-clockwise
///
/// The font uses two files, the tileset with extension ".img" and the palette
/// with extension ".pal".
///
/// You must load the font for every text layer you want to create. Every font
/// loaded uses a slot of tiled backgrounds of RAM.
///
/// There are many examples with custom fonts in the examples folder.
///
/// Example:
/// ```
/// // Load the font with files "default" from folder "stage4" and call it
/// // "titulo", rotated counter-clockwise. The text layer created is of 32x32
/// // tiles (256x256 pixels).
/// NF_LoadTextFont16("stage4/default", "titulo", 256, 256, 2);
/// ```
///
/// @param file File name
/// @param name Font name
/// @param width Map width (in pixels)
/// @param height Map height (in pixels)
/// @param rotation Rotation (0 - 2)
void NF_LoadTextFont16(const char *file, const char *name, u32 width, u32 height,
                       u32 rotation);

/// Create a special tiled background to write text on it with a 8x16 font.
///
/// You must select the screen and layer where the background will be created,
/// the orientation of the text and the font you want to use.
///
/// Example:
/// ```
/// // Create a text layer on layer 0 of screen 1, using the font with name
/// // "titulo" and with the text rotated to the left.
/// NF_CreateTextLayer16(1, 0, 2, "titulo");
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
/// @param rotation Rotation (0 - 2)
/// @param name Font name
void NF_CreateTextLayer16(int screen, u32 layer, u32 rotation, const char *name);

/// Write text in a layer with a 8x16 font at the specified coordinates.
///
/// You must specify the screen and layer where you want to write the text. The
/// text is not writen directly on the screen. It's stored in a temporary buffer
/// and it's transferred to the screen when the function NF_UpdateTextLayers()
/// is called. This is done to minimize the number of times VRAM is updated.
///
/// If you want to write variables or formated text, use snprintf().
///
/// Example 1:
/// ```
/// // Write "Hello World!" to layer 1 of the bottom screen
/// NF_WriteText16(1, 0, 1, 1, "Hello World!");
/// ```
///
/// Example 2:
/// ```
/// // Write a formatted string to layer 1 of the bottom screen
/// char buffer[32];
/// int myvar = 10;
/// snprintf(buffer, sizeof(buffer), “Hello world %d times”, myvar);
/// NF_WriteText16(1, 0, 1, 1, buffer);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
/// @param x X coordinate
/// @param y Y coordinate
/// @param text String to write to the screen
void NF_WriteText16(int screen, u32 layer, u32 x, u32 y, const char *text);

/// Clears the contents of a text layer with a 8x16 font filling it with zeroes.
///
/// Example:
/// ```
/// // Clears the contents of text layer 2 of screen 0
/// NF_ClearTextLayer16(0, 2);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
void NF_ClearTextLayer16(int screen, u32 layer);

/// @}

#endif // NF_TEXT16_H__

#ifdef __cplusplus
}
#endif
