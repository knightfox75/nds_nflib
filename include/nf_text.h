// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 8x8 font text support functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_TEXT_H__
#define NF_TEXT_H__

#include <nds.h>

/// @file   nf_text.h
/// @brief  Text support functions.

/// @defgroup nf_text Text support functions.
///
/// Text support functions of NFLIb.
///
/// @{

/// Number of characters available in a font
#define NF_TEXT_FONT_CHARS 127

/// Last valid character in a font
#define NF_TEXT_FONT_LAST_VALID_CHAR 113

/// Struct that holds text layer information
typedef struct {
    u16 width;      ///< Rightmost tile of a row (for 32 columns this is 31)
    u16 height;     ///< Bottom-most tile of a column (for 24 rows this is 23)
    u8 rotation;    ///< Text rotation
    u8 slot;        ///< Slot where the tileset of this text layer is loaded
    u8 pal;         ///< Palette used for the text (0 by default)
    bool exist;     ///< True if this layer has been loaded
    bool update;    ///< True if this layer needs to be updated
} NF_TYPE_TEXT_INFO;

/// Information of all text layers.
extern NF_TYPE_TEXT_INFO NF_TEXT[2][4];

/// Initialize the text engine for the selected screen.
///
/// You must also initialize the tiled background system of that screen before
/// using the text engine. You can get more information about it in
/// NF_InitTiledBgBuffers() and NF_InitTiledBgSys().
///
/// Use this function also to reset text system.
///
/// Example:
/// ```
/// // Initialize text engine of screen 1
/// NF_InitTextSys(1);
/// ```
///
/// @param screen Screen (0 - 1).
void NF_InitTextSys(int screen);

/// Load font and palette files from the filesystem to RAM.
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
/// NF_LoadTextFont("stage4/default", "titulo", 256, 256, 2);
/// ```
///
/// @param file File name
/// @param name Font name
/// @param width Map width (in pixels)
/// @param height Map height (in pixels)
/// @param rotation Rotation (0 - 2)
void NF_LoadTextFont(const char *file, const char *name, u32 width, u32 height,
                     u32 rotation);

/// Delete from RAM the font with the specified name.
///
/// Example:
/// ```
/// // Delete from RAM the font with name "titulo".
/// NF_UnloadTextFont("titulo");
/// ```
///
/// @param name Font name
void NF_UnloadTextFont(const char *name);

/// Create a special tiled background to write text on it.
///
/// You must select the screen and layer where the background will be created,
/// the orientation of the text and the font you want to use.
///
/// Example:
/// ```
/// // Create a text layer on layer 0 of screen 1, using the font with name
/// // "titulo" and with the text rotated to the left.
/// NF_CreateTextLayer(1, 0, 2, "titulo");
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
/// @param rotation Rotation (0 - 2)
/// @param name Font name
void NF_CreateTextLayer(int screen, u32 layer, u32 rotation, const char *name);

/// Delete a text layer.
///
/// You must specify the layer and screen of the text layer you want to delete.
///
/// Example:
/// ```
/// // Delete the text layer of layer 0 of the bottom screen.
/// NF_DeleteTextLayer(1, 0);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
void NF_DeleteTextLayer(int screen, u32 layer);

/// Write text in a layer at the specified coordinates.
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
/// NF_WriteText(1, 0, 1, 1, "Hello World!");
/// ```
///
/// Example 2:
/// ```
/// // Write a formatted string to layer 1 of the bottom screen
/// char buffer[32];
/// int myvar = 10;
/// snprintf(buffer, sizeof(buffer), “Hello world %d times”, myvar);
/// NF_WriteText(1, 0, 1, 1, buffer);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
/// @param x X coordinate
/// @param y Y coordinate
/// @param text String to write to the screen
void NF_WriteText(int screen, u32 layer, u32 x, u32 y, const char *text);

/// Copy the temporary text buffers of both screens to VRAM.
///
/// Buffers are only copied if the copy in VRAM is outdated.
///
/// Example:
/// ```
/// // Update all text layers that require it
/// NF_UpdateTextLayers();
/// ```
void NF_UpdateTextLayers(void);

/// Clears the contents of a text layer filling it with zeroes.
///
/// Example:
/// ```
/// // Clears the contents of text layer 2 of screen 0
/// NF_ClearTextLayer(0, 2);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
void NF_ClearTextLayer(int screen, u32 layer);

/// Defines a RGB color to be used later as a text color.
///
/// The color is stored in the specified slot. To make this function work, the
/// font palette must be indexed and it must have 2 colors (Magenta/White). Only
/// color index 1 is updated by this function, so any other color in higher
/// indices won't be updated.
///
/// Example:
/// ```
/// // Defines color numer 13 of text layer 0 of the top screen as light green
/// NF_DefineTextColor(0, 0, 13, 15, 31, 15);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
/// @param color Color number (0 - 15)
/// @param r Red component (0 - 31)
/// @param g Green component (0 - 31)
/// @param b Blue component (0 - 31)
void NF_DefineTextColor(int screen, u32 layer, u32 color, u32 r, u32 g, u32 b);

/// Sets the color to use in all text written after calling this function.
///
/// The text that is already on the screen won't be altered.
//
/// Example:
/// ```
/// // From now on, all text written in layer 0 of the top screen will use the
/// // color stored in slot 3.
/// NF_SetTextColor(0, 0, 3);
/// ```
///
/// @param screen Screen (0 - 1)
/// @param layer Background layer (0 - 3)
/// @param color Color number (0 - 15)
static inline void NF_SetTextColor(int screen, u32 layer, u32 color)
{
    NF_TEXT[screen][layer].pal = color;
}

/// @}

#endif // NF_TEXT_H__

#ifdef __cplusplus
}
#endif
